#pragma once
#include <GL/glew.h>
#include <string>
#include <deque>

struct _CompShader { const char* path; };
struct _FragShader { const char* path; };
struct _VertShader { const char* path; };
struct _GeomShader { const char* path; };
struct _TescShader { const char* path; };
struct _TeseShader { const char* path; };

struct NoShader;	//use this class to indicate left-out shader stages
class ProgramLowLevelBase;
template<typename Uniform_T> class ProgramBase;

template<
	typename Uni_T,					// Class implementing setting uniforms and rendering them
	typename FragComp_t,			// Shader class implementation for fragment OR compute shader, and so on...
	typename Vert_t = NoShader, typename Geom_t = NoShader,	typename TesC_t = NoShader,	typename TesE_t = NoShader>
class Program :
	protected ProgramBase<Uni_T>
{	//static_assert(std::is_base_of_v<FragComp_t, Shader<SFile>>, "Must have a proper fragment/compute shader type set");
	class LoadState;	//fwd decl
	LoadState load_state = LoadState(*this);
public:
	Program(const std::string& name = ""); //glCreateProgram :)
	Program(const char* name = "");
	~Program() = default;

	bool Link();
	const std::string& GetErrors() const { return this->getErrors(); }
	
	//For pushing uniforms
	typename ProgramBase<Uni_T>::InvalidState& operator << (const std::string &str);
	//For adding shader files. Same types will concatenate.
	inline LoadState& operator << (const _CompShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const _FragShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const _VertShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const _GeomShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const _TescShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const _TeseShader& s){ return (this->load_state << s); }
	inline LoadState& operator << (const typename LoadState::LinkType link) { return (this->load_state << link); }
	//Compile shaders and Link the program

	// This will render and update shaders. Only use it with the base program
	void Render() {}
protected:
	FragComp_t	fragcomp;	Vert_t vert;	Geom_t geom;	TesC_t tesc;	TesE_t tese;
	std::string program_name;
};

#include "detail/Program.inl"