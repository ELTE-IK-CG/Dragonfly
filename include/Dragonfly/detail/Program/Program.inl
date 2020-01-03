#pragma once
#include "../../config.h"
#include "../Program/Program.h"
#include "../Program/ProgramBase.h"
#include "../Framebuffer/Framebuffer.h"

// ========================= Program Base Classes ==============================

namespace df
{

template<typename Uniform_T> 
	class ProgramBase : protected ProgramLowLevelBase
{
private:
	class ValidState;	//fwd decl
protected:
	class InvalidState;	//fwd decl
protected:
	Uniform_T uniforms;
	ProgramBase(SubroutinesBase& sub): uniforms(program_id, sub){}
	ValidState	 valid_state   = ValidState(*this);		//	for the << trick with uniforms
	InvalidState invalid_state = InvalidState(*this);	//	only the first use binds
#ifdef _DEBUG
	bool  ended_with_valid_state = true;
#endif // _DEBUG
	inline void startUniformSetupOperator_SetNewName(const std::string& str) { invalid_state.new_name = str; };
};

// ========================= Helper classes ==============================

class SFile;	class FileEditor;
template<typename File> class Shader;
template<typename File> class ShaderEditor;

class NoUniforms {
private:
	GLuint program_id = 0;
	friend class ProgramBase<NoUniforms>;
	NoUniforms(GLuint program_id) : program_id(program_id) {}
public:
	void Render() {}
	template<typename ValType>
	void SetUniform(std::string &&str, ValType &&val)	{
		std::cout << "Program id = " <<program_id << ", uniform name: \"" << str << "\", size = " << sizeof(ValType) << std::endl;
	}
};

struct NoShader {
	NoShader(GLuint) {}
	constexpr GLuint getID() const { return 0; }
	constexpr const char* GetErrors() const { return ""; }
	constexpr bool Compile() { return true; }
	void Render(std::string program_name = "default") {}
	constexpr void Update() {}
};

static typename ProgramLowLevelBase::LinkType LinkProgram, CompileProgram;

// ========================= Program Input States ==============================

template<typename U>
class ProgramBase<U>::InvalidState
{
	friend class ProgramBase<U>;
	InvalidState() = delete;
public:
	template<typename ValueType>
	ValidState& operator <<(const ValueType& value) {

		using VT = std::remove_cv_t < std::remove_reference_t < ValueType>>;
		static_assert(!std::is_same_v < ProgramLowLevelBase::LinkType, VT>, "Invalid type in Program's << operator: cannot link in uniform mode.");
		static_assert(!(
			std::is_same_v<detail::_CompShader, VT> || std::is_same_v<detail::_FragShader, VT> || std::is_same_v<detail::_VertShader, VT> || std::is_same_v<detail::_GeomShader, VT> || std::is_same_v<detail::_TescShader, VT> || std::is_same_v<detail::_TeseShader, VT>
			), "Invalid type in Program's << operator: Shader type as input in uniform mode.");
		static_assert(!(std::is_same_v < std::string, VT> || std::is_same_v< char, std::remove_extent<std::remove_pointer_t<VT>>>
			), "Invalid type in Program's << operator: cannot set a string as a uniform.");

		ASSERT(that.program_id == bound_program_id, "Pretty hard to achive this error. Do not bind another program while adding uniforms.");
		that.uniforms.SetUniform(std::move(new_name), value);
#ifdef _DEBUG
		that.ended_with_valid_state = true;
#endif // _DEBUG
		return that.valid_state;
	}
private:
	std::string new_name;
	ProgramBase<U>& that;
	InvalidState(ProgramBase<U>& that) :that(that) {}
};
template<typename U>
class ProgramBase<U>::ValidState
{
	friend class ProgramBase<U>;
	ValidState() = delete;
public:
	InvalidState& operator <<(const std::string& str) {
		that.invalid_state.new_name = str;
#ifdef _DEBUG
		that.ended_with_valid_state = false;
#endif // _DEBUG
		return that.invalid_state;
	}
private:
	ProgramBase<U>& that;
	ValidState(ProgramBase<U>& that) :that(that) {}
};

// ========================= Program implementation ==============================

template<typename S, typename U, typename R>
	Program<S, U, R>::Program(const char* name)	: Program<S, U, R>::Program(std::string(name))
	{}

template<typename S, typename U, typename R>
	Program<S, U, R>::Program(const std::string& name) : ProgramBase<U>::ProgramBase(subroutines),
		comp(GL_COMPUTE_SHADER),
		frag(GL_FRAGMENT_SHADER), vert(GL_VERTEX_SHADER), geom(GL_GEOMETRY_SHADER), tesc(GL_TESS_CONTROL_SHADER), tese(GL_TESS_EVALUATION_SHADER),
		program_name(name),
		subroutines(ProgramLowLevelBase::program_id)
	{}

template<typename S, typename U, typename R>
	inline typename ProgramBase<U>::InvalidState&
		Program<S, U, R>::operator<<(const std::string & str)
{
	this->bind();
#ifdef _DEBUG
	ASSERT(this->ended_with_valid_state, "Last uniform upload ended with a uniform name and no value was given.");
	this->ended_with_valid_state = false;
#endif // _DEBUG
	this->startUniformSetupOperator_SetNewName(std::move(str));
	return this->invalid_state;
}

template<typename Shaders_T, typename Uni_T, typename Subroutines_T>
Program<Shaders_T, Uni_T, Subroutines_T>& df::Program<Shaders_T, Uni_T, Subroutines_T>::operator<<(const VaoArrays& vao)
{
	subroutines.SetSubroutines();
	this->draw(vao);
	return *this;
}

template<typename Shaders_T, typename Uni_T, typename Subroutines_T>
Program<Shaders_T, Uni_T, Subroutines_T>& Program<Shaders_T, Uni_T, Subroutines_T>::operator<<(const VaoElements& vao)
{
	subroutines.SetSubroutines();
	this->draw(vao);
	return *this;
}

} //namespace df

#include "ProgramCompile.inl"

