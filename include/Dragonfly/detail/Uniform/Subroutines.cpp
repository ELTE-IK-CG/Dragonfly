#include "Subroutines.h"

#include <imgui/imgui.h>
#include <Dragonfly/config.h>

#include <sstream>

using namespace df;

#define NAME_LENGTH 64

ShaderSubroutines::ShaderSubroutines(GLuint program, GLenum shadertype)
	: program(program), shadertype(shadertype)
{}

void ShaderSubroutines::Compile()
{
	subNames.clear();
	uniforms.clear();
	indices.clear();

	// TODO GL_CHEKS everywhere?
	GLint num;
	GLchar buff[NAME_LENGTH];

	// get subroutines
	glGetProgramStageiv(program, shadertype, GL_ACTIVE_SUBROUTINES, &num);
	subNames.reserve(num);

	for (GLint ind = 0; ind < num; ++ind) {
		glGetActiveSubroutineName(program, shadertype, ind, NAME_LENGTH, nullptr, buff);
		std::stringstream ss;
		ss << buff;
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

void ShaderSubroutines::RenderUI()
{
	ImGui::PushID(shadertype);
	if (ImGui::CollapsingHeader("shader stage ???")) { // TODO write shader type
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
	ImGui::PopID();
}

bool ShaderSubroutines::SubroutineSelector(const std::vector<GLint>& compatibleSubs, GLuint& subIndex)
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


bool SubroutinesBase::Compile()
{
	uniIndices.clear();
	subInds.clear();

	uint8_t shaderInd = 0;
	for (auto& sub : shaderSubs) {
		sub.Compile();

		for (size_t i = 0; i < sub.uniforms.size(); ++i) {
			auto& uni = sub.uniforms[i];
			WARNING(uniIndices.find(uni.name) != uniIndices.end(), "Subroutines: subroutine uniform name used in multiple shader stages, we only allow unique names");
			uniIndices[uni.name] = { shaderInd, i };
		}
		for (size_t i = 0; i < sub.subNames.size(); ++i) {
			subInds[{shaderInd, sub.subNames[i]}] = static_cast<GLuint>(i);
		}

		++shaderInd;
	}

	return true;
}

bool SubroutinesBase::SetSubroutine(const std::string & uniform, const std::string & subroutine)
{
	auto uniIt = uniIndices.find(uniform);
	if (uniIt == uniIndices.end()) {
		return false;
	}
	size_t uniIndex = uniIt->second.second;
	uint8_t shaderInd = uniIt->second.first;

	auto subIt = subInds.find(std::pair<uint8_t, std::string>(shaderInd, subroutine));
	if (subIt == subInds.end()) {
		return false;
	}
	GLuint subInd = subIt->second;

	const auto& compatible = shaderSubs[shaderInd].uniforms[uniIndex].compatibleSubs;
	if (std::find(compatible.begin(), compatible.end(), subInd) == compatible.end()) { // TODO binary_search, are they in order?
		return false;
	}
	GLint uniLoc = shaderSubs[shaderInd].uniforms[uniIndex].loc;

	shaderSubs[shaderInd].indices[uniLoc] = subInd;

	return true;
}

void SubroutinesBase::SetSubroutines()
{
	for (auto& sub : shaderSubs)
		sub.SetSubroutines();
}

bool SubroutinesBase::HasUniform(const std::string & uniform) const
{
	return uniIndices.find(uniform) != uniIndices.end();
}

void SubroutinesBase::Render(const std::string& program_name)
{
	if(ImGui::Begin(program_name.c_str())) {
		for (auto& sub : shaderSubs)
			sub.RenderUI();
	} ImGui::End();

}
