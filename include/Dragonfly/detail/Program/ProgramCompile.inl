#pragma once
#include "../../config.h"
#include "../Program/Program.h"

// ========================= Program Base Classes ==============================

constexpr df::detail::_CompShader operator"" _comp(const char* str, size_t len) { return df::detail::_CompShader{ str }; }
constexpr df::detail::_FragShader operator"" _frag(const char* str, size_t len) { return df::detail::_FragShader{ str }; }
constexpr df::detail::_VertShader operator"" _vert(const char* str, size_t len) { return df::detail::_VertShader{ str }; }
constexpr df::detail::_GeomShader operator"" _geom(const char* str, size_t len) { return df::detail::_GeomShader{ str }; }
constexpr df::detail::_TescShader operator"" _tesc(const char* str, size_t len) { return df::detail::_TescShader{ str }; }
constexpr df::detail::_TeseShader operator"" _tese(const char* str, size_t len) { return df::detail::_TeseShader{ str }; }

namespace df
{

constexpr detail::_FragShader operator"" _fs  (const char* str, size_t len) { return detail::_FragShader{ str }; }
constexpr detail::_VertShader operator"" _vs  (const char* str, size_t len) { return detail::_VertShader{ str }; }

// ========================= Program Input States ==============================

//This state ensures you cannot add uniforms while adding files.
template<typename S, typename U, typename R>
class Program<S, U, R>::LoadState {
	using Prog = Program<S, U, R>;
	friend class Prog;
	LoadState() = delete;
public:
	struct LinkType {};
	LoadState& operator << (const detail::_CompShader& s);
	LoadState& operator << (const detail::_FragShader& s);
	LoadState& operator << (const detail::_VertShader& s);
	LoadState& operator << (const detail::_GeomShader& s);
	LoadState& operator << (const detail::_TescShader& s);
	LoadState& operator << (const detail::_TeseShader& s);
	LoadState& operator << (const ProgramLowLevelBase::LinkType &andCompile);
private:
	LoadState(Prog&that) : that(that) {}
	Prog& that;
	std::deque<const char*> shader_list;
};

template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_CompShader & s) {
	static_assert(!std::is_same_v<S::Comp, NoShader>, "Program: No compute shader is present. Maybe you have vertex and fragment shaders instead.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.comp << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_FragShader& s) {
	static_assert(!std::is_same_v<S::Frag, NoShader>, "Program: No fragment shader is present. Maybe you have a compute program.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.frag << s.path;
	}
	else 
		 WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_VertShader & s) {
	static_assert(!std::is_same_v<S::Vert, NoShader>, "Program: No vertex shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.vert << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_GeomShader & s) {
	static_assert(!std::is_same_v<S::Geom, NoShader>, "Program: No geometry shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.geom << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_TescShader & s) {
	static_assert(!std::is_same_v<S::TesC, NoShader>, "Program: No tessellation control shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.tesc << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const detail::_TeseShader & s) {
	static_assert(!std::is_same_v<S::TesE, NoShader>, "Program: No tessellation evaluation shader is present.");
	if (std::find(shader_list.begin(), shader_list.end(), s.path) == shader_list.end()) {
		shader_list.push_back(s.path); that.tese << s.path;
	}
	else
		WARNING(true, ("More than one occurenc of a shader file detected, extra item removed: " + std::string(s.path)).c_str() );
	return *this;
}
template<typename S, typename U, typename R>
inline typename Program<S, U, R>::LoadState& Program<S, U, R>::LoadState::operator<<(const ProgramLowLevelBase::LinkType &andCompile) {
	that.Link();		return *this;
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
