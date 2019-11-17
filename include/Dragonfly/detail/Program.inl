#pragma once
#include "../config.h"
#include "../Program.h"

// ========================= Program Base Classes ==============================

namespace df
{

class ProgramLowLevelBase
{
protected:
	static GLuint bound_program_id;
	GLuint program_id = 0;
	std::string error_msg;
	bool link();
	inline void bind() {
		if (bound_program_id != program_id) {
			glUseProgram(program_id);
			bound_program_id = program_id;
		}
	}
	ProgramLowLevelBase();
	~ProgramLowLevelBase();

	ProgramLowLevelBase(const ProgramLowLevelBase&) = delete;
	ProgramLowLevelBase& operator=(const ProgramLowLevelBase&) = delete;

	ProgramLowLevelBase(ProgramLowLevelBase&& rhs);
	ProgramLowLevelBase& operator=(ProgramLowLevelBase&& rhs);

	template<typename Shader_t>
	void attachShader(const Shader_t &sh) {
		if (sh.getID() != 0) glAttachShader(program_id, sh.getID());
	}
public:
	struct LinkType {};	//contains nothing at all
	const std::string& getErrors() const;
};

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
static typename SetSubroutinesType SetSubroutines; // TODO remove this if possible

constexpr _CompShader operator"" _comp(const char* str, size_t len) { return _CompShader{ str }; }
constexpr _FragShader operator"" _frag(const char* str, size_t len) { return _FragShader{ str }; }
constexpr _VertShader operator"" _vert(const char* str, size_t len) { return _VertShader{ str }; }
constexpr _GeomShader operator"" _geom(const char* str, size_t len) { return _GeomShader{ str }; }
constexpr _TescShader operator"" _tesc(const char* str, size_t len) { return _TescShader{ str }; }
constexpr _TeseShader operator"" _tese(const char* str, size_t len) { return _TeseShader{ str }; }
constexpr _FragShader operator"" _fs  (const char* str, size_t len) { return _FragShader{ str }; }
constexpr _VertShader operator"" _vs  (const char* str, size_t len) { return _VertShader{ str }; }


// ========================= Program Input States ==============================

template<typename U>
class ProgramBase<U>::	InvalidState
{
	friend class ProgramBase<U>;
	InvalidState() = delete;
public:
	template<typename ValueType>
	ValidState& operator <<(const ValueType &value) {

		using VT = std::remove_cv_t < std::remove_reference_t < ValueType>>;
		static_assert(!std::is_same_v < ProgramLowLevelBase::LinkType, VT>, "Invalid type in Program's << operator: cannot link in uniform mode.");
		static_assert(!(
			std::is_same_v<_CompShader, VT> || std::is_same_v<_FragShader, VT> || std::is_same_v<_VertShader, VT> || std::is_same_v<_GeomShader, VT> || std::is_same_v<_TescShader, VT> || std::is_same_v<_TeseShader, VT>
			), "Invalid type in Program's << operator: Shader type as input in uniform mode.");
		static_assert(!( std::is_same_v < std::string, VT> || std::is_same_v< char, std::remove_extent<std::remove_pointer_t<VT>>>
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
	ProgramBase<U> &that;
	InvalidState(ProgramBase<U> &that) :that(that) {}
};
template<typename U>
class ProgramBase<U>::	ValidState
{
	friend class ProgramBase<U>;
	ValidState() = delete;
public:
	InvalidState& operator <<(const std::string & str) {
		that.invalid_state.new_name = str;
#ifdef _DEBUG
		that.ended_with_valid_state = false;
#endif // _DEBUG
		return that.invalid_state;
	}
private:
	std::string new_name;
	ProgramBase<U> &that;
	ValidState(ProgramBase<U> &that) :that(that) {}
};

//This state ensures you cannot add uniforms while adding files.
template<typename S, typename U, typename R>
class Program<S, U, R>::LoadState {
	using Prog = Program<S, U, R>;
	friend class Prog;
	LoadState() = delete;
public:
	struct LinkType {};
	LoadState& operator << (const _CompShader& s);
	LoadState& operator << (const _FragShader& s);
	LoadState& operator << (const _VertShader& s);
	LoadState& operator << (const _GeomShader& s);
	LoadState& operator << (const _TescShader& s);
	LoadState& operator << (const _TeseShader& s);
	LoadState& operator << (const ProgramLowLevelBase::LinkType andCompile);
private:
	LoadState(Prog&that) : that(that) {}
	Prog& that;
	std::deque<const char*> shader_list;
};


template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _CompShader & s) {
	static_assert(!std::is_same_v<Comp_t, NoShader>, "Program: No compute shader is present. Maybe you have vertex and fragment shaders instead.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.comp << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _FragShader& s) {
	static_assert(!std::is_same_v<Frag_t, NoShader>, "Program: No fragment shader is present. Maybe you have a compute program.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.frag << s.path;
	}
	else 
		 WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _VertShader & s) {
	static_assert(!std::is_same_v<Vert_t, NoShader>, "Program: No vertex shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.vert << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _GeomShader & s) {
	static_assert(!std::is_same_v<Geom_t, NoShader>, "Program: No geometry shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.geom << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _TescShader & s) {
	static_assert(!std::is_same_v<TesC_t, NoShader>, "Program: No tessellation control shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.tesc << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const _TeseShader & s) {
	static_assert(!std::is_same_v<TesE_t, NoShader>, "Program: No tessellation evaluation shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.tese << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const typename ProgramLowLevelBase::LinkType andCompile) {
	that.Link();		return *this;
}

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


template<typename S, typename U, typename R>
inline bool	Program<S, U, R>::Link()
{	//TODO make it smart, only compile when something changed. Not sure how...
	this->error_msg.clear();
	if (!this->comp.Compile()) {
		this->error_msg += "\nCompute Shader did not compile.\n";
		this->error_msg += this->comp.GetErrors();
		return false;
	}
	if (!this->frag.Compile()) {
		this->error_msg += "\nFragment Shader did not compile.\n";
		this->error_msg += this->frag.GetErrors();
		return false;
	}
	if (!this->vert.Compile()){
		this->error_msg += "\nVertex Shader did not compile.\n";
		this->error_msg += this->vert.GetErrors();
		return false;
	}
	if (!this->geom.Compile()){
		this->error_msg += "\nGeometry Shader did not compile.\n";
		this->error_msg += this->geom.GetErrors();
		return false;
	}
	if (!this->tesc.Compile()){
		this->error_msg += "\nTessellation Control Shader did not compile.\n";
		this->error_msg += this->tesc.GetErrors();
		return false;
	}
	if (!this->tese.Compile()){
		this->error_msg += "\nTessellation Evaluation Shader did not compile.\n";
		this->error_msg += this->tese.GetErrors();
		return false;
	}
	GL_CHECK;
	this->attachShader(comp);
	this->attachShader(frag);
	this->attachShader(vert);
	this->attachShader(geom);
	this->attachShader(tesc);
	this->attachShader(tese);
	GL_CHECK;
	if (!this->link()){
		this->error_msg += "\nShader Program did not Link.\n";
		return false;
	}
	if (!this->uniforms.Compile()) {
		this->error_msg += "\n Weird error with uniforms. Uniforms class did not Compile.\n";
		return false;
	}
	if (!this->subroutines.Compile()) {
		this->error_msg += "\n Weird error with subroutines. Subroutines class did not Compile.\n";
		return false;
	}
	GL_CHECK;
#ifdef _DEBUG
	std::cout << "Program compilation was succesful.\n";
#endif // _DEBUG
	return true;
}

} //namespace df
