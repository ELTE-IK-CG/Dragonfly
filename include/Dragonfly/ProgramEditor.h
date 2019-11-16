#pragma once
#include <string>
#include "Program.h"
#include <ImGui/imgui.h>

namespace df
{

template< typename U, typename FC, typename V = NoShader, typename G = NoShader, typename TC = NoShader, typename TE = NoShader>
class ProgramEditor : public Program<U, FC, V, G, TC, TE> {
	using Base = Program<U, FC, V, G, TC, TE>;
public:
	ProgramEditor(const std::string& name) : Base(name) {}
	ProgramEditor(const char* name) : Base(name) {}
	~ProgramEditor() = default;
	void Render();
};

template<typename U, typename FC, typename V, typename G, typename TC, typename TE>
inline void ProgramEditor<U, FC, V, G, TC, TE>::Render()
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
			if (ImGui::BeginTabItem("Fragment editor")) {
				tab = 0;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Vertex editor")) {
				tab = 1;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Uniform editor")) {
				tab = 2;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}ImGui::End();

	switch (tab) {
	case(0):
		this->fragcomp.Render(this->program_name);	this->fragcomp.Update(); break;
	case(1):
		this->vert.Render(this->program_name);		this->vert.Update(); break;
	case(2):
		this->uniforms.Render(this->program_name); break;
	default:
		break;
	}
	
	this->geom.Render();		this->geom.Update();
	this->tesc.Render();		this->tesc.Update();
	this->tese.Render();		this->tese.Update();	
}

class UniformEditor;

using ShaderProgramEditorVF = ProgramEditor<UniformEditor, ShaderEditor<SFile>, ShaderEditor<SFile>, NoShader, NoShader, NoShader>;
using ShaderProgramEditorVGF = ProgramEditor<UniformEditor, ShaderEditor<SFile>, ShaderEditor<SFile>, ShaderEditor<SFile>, NoShader, NoShader>;
using ShaderProgramEditorVTF = ProgramEditor<UniformEditor, ShaderEditor<SFile>, ShaderEditor<SFile>, NoShader, ShaderEditor<SFile>, ShaderEditor<SFile>>;
using ShaderProgramEditorVGTF = ProgramEditor<UniformEditor, ShaderEditor<SFile>, ShaderEditor<SFile>, ShaderEditor<SFile>, ShaderEditor<SFile>, ShaderEditor<SFile>>;
using ComputeProgramEditor = ProgramEditor<UniformEditor, ShaderEditor<SFile>, NoShader, NoShader, NoShader, NoShader>;

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