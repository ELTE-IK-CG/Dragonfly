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
	this->fragcomp.Render(this->program_name);	this->fragcomp.Update();
	this->vert.Render(this->program_name);		this->vert.Update();
	this->geom.Render();		this->geom.Update();
	this->tesc.Render();		this->tesc.Update();
	this->tese.Render();		this->tese.Update();
	this->uniforms.Render(this->program_name);
}
