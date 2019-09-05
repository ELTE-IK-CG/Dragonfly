#pragma once
#include <string>
#include "Program.h"


template< typename U, typename FC, typename V = NoShader, typename G = NoShader, typename TC = NoShader, typename TE = NoShader>
class ProgramEditor : public Program<U, FC, V, G, TC, TE> {
	using Base = Program<U, FC, V, G, TC, TE>;
public:
	ProgramEditor(const char* name) : Base(), program_name(name) {}
	~ProgramEditor() = default;

	void Render();
private:
	std::string program_name;
};

template<typename U, typename FC, typename V, typename G, typename TC, typename TE>
inline void ProgramEditor<U, FC, V, G, TC, TE>::Render()
{
	this->fragcomp.Render(program_name);	this->fragcomp.Update();
	this->vert.Render();		this->vert.Update();
	this->geom.Render();		this->geom.Update();
	this->tesc.Render();		this->tesc.Update();
	this->tese.Render();		this->tese.Update();
	this->uniforms.Render(program_name);
}
