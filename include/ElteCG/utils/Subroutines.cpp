#include "Subroutines.h"

#include <imgui/imgui.h>


#include <sstream>

#define NAME_LENGTH 64

void Subroutines::Init(GLuint program, GLenum shadertype)
{
	subNames.clear();
	uniforms.clear();
	indices.clear();

	this->program = program;
	this->shadertype = shadertype;

	// TODO GL_CHEKS everywhere?
	GLint num;
	GLchar buff[NAME_LENGTH];

	// get subroutines
	glGetProgramStageiv(program, shadertype, GL_ACTIVE_SUBROUTINES, &num);
	subNames.reserve(num);

	for (GLint ind = 0; ind < num; ++ind) {
		glGetActiveSubroutineName(program, shadertype, ind, NAME_LENGTH, nullptr, buff);
		std::stringstream ss;
		ss << ind << " " << buff;
		subNames.push_back(ss.str());
	}

	// get uniforms
	glGetProgramStageiv(program, shadertype, GL_ACTIVE_SUBROUTINE_UNIFORMS, &num);
	uniforms.resize(num);

	for (GLint ind = 0; ind < num; ++ind) {
		SubUniform& uni = uniforms[ind];
		GLint compNum;
		glGetActiveSubroutineUniformiv(program, shadertype, ind, GL_NUM_COMPATIBLE_SUBROUTINES, &compNum);
		uni.compatibleSubs.resize(compNum);
		glGetActiveSubroutineUniformiv(program, shadertype, ind, GL_COMPATIBLE_SUBROUTINES, &uni.compatibleSubs[0]);
		glGetActiveSubroutineUniformiv(program, shadertype, ind, GL_UNIFORM_SIZE, &uni.size);
		glGetActiveSubroutineUniformName(program, shadertype, ind, NAME_LENGTH, nullptr, buff);
		// TODO if array remove [0]?
		uni.name = buff;
		uni.loc = glGetSubroutineUniformLocation(program, shadertype, buff);
	}

	// init indices array
	glGetProgramStageiv(program, shadertype, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &num);
	indices.resize(num);

	// set compatible indices
	for (const SubUniform& uni : uniforms) {
		for (int i = 0; i < uni.size; ++i) {
			indices[uni.loc + i] = uni.compatibleSubs[0];
		}
	}
}

bool Subroutines::RenderUI()
{
	if (ImGui::Begin("Subroutines", &uiIsOpen))
	{
		for (const auto& x : subNames) 
		{
			ImGui::Text("%s", x.c_str());
		}
		for (SubUniform& uni : uniforms)
		{
			ImGui::PushID(uni.loc);
			ImGui::Text("loc: %i, name: %s", uni.loc, uni.name.c_str());
			for (GLint i = 0; i < uni.size; ++i)
			{
				ImGui::Text("%i", i);
				ImGui::SameLine();
				ImGui::PushID(i);
				SubroutineSelector(uni.compatibleSubs, indices[uni.loc + i]);
				ImGui::PopID();
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
	return uiIsOpen;
}

void Subroutines::SetSubroutines() const
{
	// TODO gl_checks?
	if(!indices.empty())
		glUniformSubroutinesuiv(shadertype, static_cast<GLsizei>(indices.size()), &indices[0]);
}

bool Subroutines::SubroutineSelector(const std::vector<GLint>& compatibleSubs, GLuint& subIndex)
{
	// mostly copied from ImGui::Combo()

	ImGuiContext& g = *ImGui::GetCurrentContext();

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (subIndex >= 0 && subIndex < subNames.size())
		preview_value = subNames[subIndex].c_str();

	if (!ImGui::BeginCombo("##Combo", preview_value, ImGuiComboFlags_None))
		return false;

	// Display items
	bool value_changed = false;
	for (GLint i : compatibleSubs)
	{
		ImGui::PushID((void*)(intptr_t)i);
		const bool item_selected = (i == subIndex);
		const char* item_text = subNames[i].c_str();
		if (ImGui::Selectable(item_text, item_selected))
		{
			value_changed = true;
			subIndex = i;
		}
		if (item_selected)
			ImGui::SetItemDefaultFocus();
		ImGui::PopID();
	}

	ImGui::EndCombo();
	return value_changed;
}

void CachedSubroutines::Init(GLuint program, GLenum shadertype)
{
	Subroutines::Init(program, shadertype);

	for (const auto& uni : uniforms) {
		uniLocs[uni.name] = uni.loc;
	}
	for (size_t i = 0; i < subNames.size(); ++i) {
		subInds[subNames[i]] = static_cast<GLuint>(i);
	}
}

bool CachedSubroutines::SetSubroutine(const std::string& uniform, const std::string& subroutine)
{
	auto uniIt = uniLocs.find(uniform);
	auto subIt = subInds.find(subroutine);

	if (uniIt == uniLocs.end() || subIt == subInds.end())
		return false;

	indices[uniIt->second] = subIt->second;
	return true;
}
