#pragma once
#include <string>
#include "Program.h"
#include "detail/Shader/ShaderFwd.h"
#include <ImGui/imgui.h>

namespace df
{

template<typename S, typename U, typename R = SubroutinesEditor<S>>
class ProgramEditor : public Program<S, U, R> {
	using Base = Program<S, U, R>;
public:
	ProgramEditor(const std::string& name) : Base(name) {}
	ProgramEditor(const char* name) : Base(name) {}
	~ProgramEditor() = default;
	void Render();
};

template<typename S, typename U, typename R>
inline void ProgramEditor<S, U, R>::Render()
{
	this->bind();
	int tab = -1;
	ImGui::SetNextWindowSize({ 600,400 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(this->program_name.c_str()))
	{
		if (ImGui::Button("Compile & Link"))
		{
			this->Link();
		}
		if (ImGui::BeginTabBar("ProgramTabBar", 0)) {
			if constexpr (std::is_same_v<typename S::Frag, NoShader>) {
				if (ImGui::BeginTabItem("Compute shader editor")) {
					tab = 0;
					ImGui::EndTabItem();
				}
			}
			else {
				if (ImGui::BeginTabItem("Fragment editor")) {
					tab = 1;
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Vertex editor")) {
					tab = 2;
					ImGui::EndTabItem();
				}
			}
			if (ImGui::BeginTabItem("Uniform editor")) {
				tab = 3;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Subroutine editor")) {
				tab = 4;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}ImGui::End();

	switch (tab) {
	case(0):
		this->comp.Render(this->program_name);	this->comp.Update(); break;
	case(1):
		this->frag.Render(this->program_name);	this->frag.Update(); break;
	case(2):
		this->vert.Render(this->program_name);	this->vert.Update(); break;
	case(3):
		this->uniforms.Render(this->program_name); break;
	case(4):
		this->subroutines.Render(this->program_name); break;
	default:
		break;
	}
	
	this->geom.Render();		this->geom.Update();
	this->tesc.Render();		this->tesc.Update();
	this->tese.Render();		this->tese.Update();	
}

class UniformEditor;

using ShaderProgramEditorVF = ProgramEditor<ShaderEditorVF, UniformEditor>;
using ShaderProgramEditorVGF = ProgramEditor<ShaderEditorVGF, UniformEditor>;
using ShaderProgramEditorVTF = ProgramEditor<ShaderEditorVTF, UniformEditor>;
using ShaderProgramEditorVGTF = ProgramEditor<ShaderEditorVGTF, UniformEditor>;
using ComputeProgramEditor = ProgramEditor<ShaderEditorCompute, UniformEditor>;

/*
inline ComputeProgramEditor ComputeProgramEditor(const std::string& name, const std::string& comp)
{
	ComputeProgramEditor prog(name);
	prog << _CompShader{ comp.c_str() };
	return prog;
}
inline ShaderProgramEditorVF ShaderProgramEditor(const std::string& name, const std::string& vert, const std::string& frag)
{
	ShaderProgramEditorVF prog(name);
	prog << _VertShader{ vert.c_str() } << _FragShader{ frag.c_str() };
	return prog;
}
inline ShaderProgramEditorVGF ShaderProgramEditor(const std::string& name, const std::string& vert, const std::string& geom, const std::string& frag)
{
	ShaderProgramEditorVGF prog(name);
	prog << _VertShader{ vert.c_str() } << _GeomShader{geom.c_str()} << _FragShader{ frag.c_str() };
	return prog;
}
inline ShaderProgramEditorVTF ShaderProgramEditor(const std::string& name, const std::string& vert,
									const std::string& tess_control,const std::string& tess_evaluation, const std::string& frag)
{
	ShaderProgramEditorVTF prog(name);
	prog << _VertShader{ vert.c_str() } << _TescShader{tess_control.c_str()} << _TeseShader{tess_evaluation.c_str()} << _FragShader{ frag.c_str() };
	return prog;
}
inline ShaderProgramEditorVGTF ShaderProgramEditor(const std::string& name, const std::string& vert, const std::string& geom,
									const std::string& tess_control, const std::string& tess_evaluation, const std::string& frag)
{
	ShaderProgramEditorVGTF prog(name);
	prog << _VertShader{ vert.c_str() } << _GeomShader{geom.c_str()} << _TescShader{tess_control.c_str()} << _TeseShader{tess_evaluation.c_str()} << _FragShader{ frag.c_str() };
	return prog;
}
*/

} // namespace df