#pragma once
#include <string>
#include "Program.h"


template< typename U, typename FC, typename V = NoShader, typename G = NoShader, typename TC = NoShader, typename TE = NoShader>
class ProgramEditor : public Program<U, FC, V, G, TC, TE> {
	using Base = Program<U, FC, V, G, TC, TE>;
public:
	ProgramEditor(const char* name) : Base(name) {}
	~ProgramEditor() = default;

	//void Render();
private:

};
