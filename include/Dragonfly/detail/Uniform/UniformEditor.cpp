#include "UniformEditor.h"
#include <ImGui/imgui.h>
#include <ImGui-addons/auto/auto.h>
#include <algorithm>
#include <iostream>

using namespace df;

IMGUI_AUTO_DEFINE_INLINE(template<>, UniformLowLevelBase::uni_hash_type, ImGui::Text("%s%u", (name.empty() ? "" : name + "=").c_str(), var.h);)

void UniformEditor::Render(const std::string &program_name){
	if( ImGui::Begin(program_name.c_str())) {
		if (ImGui::BeginChild("Uniforms", ImGui::GetContentRegionAvail(), true)) {
			const ImVec2 region = ImGui::GetContentRegionAvail();
			const float positions[] = { 30.f + region.x * 0.01f, 50.f + region.x * 0.04f, 120.f + region.x * 0.05f, 310.f + region.x * 0.05f };
			ImGui::TextUnformatted("(?)");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Ignore input | Uniform location");
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 0));
			if (ImGui::ArrowButton("##lu", ImGuiDir_Up)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool { return loc2data.at(a).loc < loc2data.at(b).loc; });
			ImGui::SameLine();
			if (ImGui::ArrowButton("##ld", ImGuiDir_Down)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool {return loc2data.at(a).loc > loc2data.at(b).loc; });
			ImGui::SameLine(positions[1]);
			ImGui::TextColored({ 0.25f,0.25f,1,1 }, "Type");
			ImGui::SameLine();
			if (ImGui::ArrowButton("##tu", ImGuiDir_Up)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool {return loc2data.at(a).gpu_type < loc2data.at(b).gpu_type; });
			ImGui::SameLine();
			if (ImGui::ArrowButton("##td", ImGuiDir_Down)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool {return loc2data.at(a).gpu_type > loc2data.at(b).gpu_type; });
			ImGui::SameLine(positions[2]);
			ImGui::TextColored({ 0.3f,1,1,1 }, "Uniform name");
			ImGui::SameLine();
			if (ImGui::ArrowButton("##nu", ImGuiDir_Up)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool {return loc2data.at(a).name < loc2data.at(b).name; });
			ImGui::SameLine();
			if (ImGui::ArrowButton("##nd", ImGuiDir_Down)) std::stable_sort(loc_order.begin(), loc_order.end(),
				[&](GLint a, GLint b)->bool {return loc2data.at(a).name > loc2data.at(b).name; });
			ImGui::PopStyleVar(2);
			ImGui::SameLine(positions[3]);
			ImGui::TextColored({ 1,0.9f,1,1 }, "Uniform value");
			ImGui::Separator();

			ImGui::PushItemWidth(-1.f);
			for (GLint loc : loc_order)
			{
				ImGui::PushID(loc);
				UniformData& d = loc2data.at(loc);
				ImGui::Separator();
				ImGui::Checkbox("##igin", &d.ignore_input);
				ImGui::SameLine(positions[0]);
				ImGui::Text("%i", d.loc);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("index = %i", d.idx);
				ImGui::SameLine(positions[1]);
				ImGui::TextColored({ 0.25f,0.25f,1,1 }, "%s", d.gpu_type.c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", d.cpu_type.c_str());
				ImGui::SameLine(120.f);
				ImGui::SameLine(positions[2]);
				ImGui::TextColored({ 0.3f,1,1,1 }, "%s", d.name.c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("gpu_size=%i, cpu_size=%i", d.gpu_size, d.gpu_size);
				ImGui::SameLine(positions[3]);
				{
					if (d.ignore_input)
						std::visit([&](auto&& arg) {
						ImGui::Auto(arg, "");
						if (ImGui::IsItemEdited())
							this->SetUni(d.loc, arg);
							}, d.variant);
					else
						std::visit([](auto&& arg) {
						ImGui::Auto(std::as_const(arg), "");
							}, d.variant);
					/*if (d.input_var == nullptr)
					{}
					else
						std::visit([&](auto&& arg) {
						ImGui::Auto(arg, "");
						if (ImGui::IsItemEdited())
						{
							this->SetUni(d.loc, arg);
							memcpy(d.input_var, static_cast<void*>(&arg), d.cpu_size);
						}
							}, d.variant);*/
				}
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
		}ImGui::EndChild();
		
	}	ImGui::End();

}

bool UniformEditor::Compile()
{
	Uniforms::Compile();
	//We have to use the parent code to be able to maintain the code
	//However, Greedy uniforms has to be fast -- both in debug and (especially) in release
	//Unfortunatelly, this means this function is very similar and does basically the same stuff
	// -- it seems like there is an easy fix for simplification: don't waste your time on it: (here, simplification will make it slower or harder to maintain)

	GLint uni_max_name_len = 0, uni_num = static_cast<GLint>(this->locations.size());
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uni_max_name_len);
	std::vector<char> namebuff(uni_max_name_len);
	loc2data.reserve(this->locations.size()); loc_order.reserve(uni_num);
	for (int i = 0; i < uni_num; ++i) {
		GLsizei size; GLenum type;
		glGetActiveUniform(program_id, (GLuint)i, (GLsizei)uni_max_name_len, nullptr, &size, &type, namebuff.data());

		GLuint loc = glGetUniformLocation(program_id, namebuff.data());
		loc_order.emplace_back(loc);

		UniformData dats(namebuff.data(), type, i, loc, size);
		ASSERT(dats.loc >= 0, ("The uniform \"" + dats.gpu_type + ' ' + dats.name +"\" does not have a location.").c_str());
		loc2data.emplace(dats.loc, dats);
		//	WARNING(vals.cpu_size != vals.gpu_size, ("The uniform \"" + vals.gpu_type + ' ' + vals.name + "\" has different size on the cpu (" + std::to_string(vals.cpu_size) +") then on the gpu (" + std::to_string(vals.gpu_size) + ").").c_str());
	}
	loc2data.rehash(0); loc_order.shrink_to_fit();
	return true;
}

UniformEditor::UniformData::UniformData(const char* name, GLenum type, GLint idx, GLint loc, GLsizei size)
	: OpenGL_BaseType(type), gpu_type(GetTypeNameFromOpenGlType(type)), name(name), idx(idx), loc(loc), gpu_size(size)
{
	std::visit([&](auto&& arg) { cpu_type = typeid(arg).name(); cpu_size = sizeof(arg); }, variant);
}
