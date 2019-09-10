#pragma once
#include <string>
#include "Program.h"


template< typename U, typename FC, typename V = NoShader, typename G = NoShader, typename TC = NoShader, typename TE = NoShader>
class ProgramEditor : public Program<U, FC, V, G, TC, TE> {
	using Base = Program<U, FC, V, G, TC, TE>;
public:
	ProgramEditor(const char* name) : Base(name) {}
	~ProgramEditor() = default;

	void Render();
};

template<typename U, typename FC, typename V, typename G, typename TC, typename TE>
inline void ProgramEditor<U, FC, V, G, TC, TE>::Render()
{
	int tab = -1;
	if (ImGui::Begin(this->program_name.c_str()))
	{
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
