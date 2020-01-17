#pragma once
#include <GL/glew.h>
#include <string>
#include <deque>
#include "../Vao/Vao.h"
#include "../Program/ProgramFwd.h"
#include "../Shader/Shader.h"
#include "../Uniform/Subroutines.h"

namespace df
{

template<
	typename Shaders_T,			// Shader types in Shaders_T::Comp, ::Frag, ect.
	typename Uni_T,				// Class implementing setting uniforms and rendering them
	typename Subroutines_T>
class Program :
	protected ProgramBase<Uni_T>
{
	friend class FramebufferBase;
	class LoadState;    LoadState    load_state	   = LoadState(*this);
public:
	Program(const std::string& name = ""); //glCreateProgram :)
	Program(const char* name = "");
	~Program() = default;

	bool Link();
	const std::string& GetErrors() const { return this->getErrors(); }
	
	//For pushing uniforms
	typename ProgramBase<Uni_T>::InvalidState& operator << (const std::string &str);

	//For rendering
	Program& operator << (const VaoElements& vao);
	Program& operator << (const VaoArrays& vao);

	//For adding shader files. Same types will concatenate.
	LoadState& operator << (const detail::_CompShader& s){ return (this->load_state << s); }
	LoadState& operator << (const detail::_FragShader& s){ return (this->load_state << s); }
	LoadState& operator << (const detail::_VertShader& s){ return (this->load_state << s); }
	LoadState& operator << (const detail::_GeomShader& s){ return (this->load_state << s); }
	LoadState& operator << (const detail::_TescShader& s){ return (this->load_state << s); }
	LoadState& operator << (const detail::_TeseShader& s){ return (this->load_state << s); }

	//Compile shaders and Link the program
	LoadState& operator << (const typename LoadState::LinkType link) { return (this->load_state << link); }
	
	// This will render and update shaders. Only use it with the base program
	void Render() {}
protected:
	typename Shaders_T::Comp comp;
	typename Shaders_T::Frag frag;
	typename Shaders_T::Vert vert;
	typename Shaders_T::Geom geom;
	typename Shaders_T::TesC tesc;
	typename Shaders_T::TesE tese;
	std::string program_name;
	Subroutines_T subroutines;
};

} //namespace df

#include "Program.inl"
