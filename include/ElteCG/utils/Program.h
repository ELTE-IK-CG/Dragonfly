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

template<typename FragComp_t, typename Vert_t = NoShader, typename Geom_t = NoShader, typename TesC_t = NoShader, typename TesE_t = NoShader>
struct Shaders
{
	using Comp = NoShader;
	using Frag = FragComp_t;
	using Vert = Vert_t;
	using Geom = Geom_t;
	using TesC = TesC_t;
	using TesE = TesE_t;
};

template<typename Compute_t>
struct Shaders<Compute_t, NoShader, NoShader, NoShader, NoShader>
{
	using Comp = Compute_t;
	using Frag = NoShader;
	using Vert = NoShader;
	using Geom = NoShader;
	using TesC = NoShader;
	using TesE = NoShader;
};

template<typename Shaders_t>
struct Subroutines {}; //todo include

template<
	typename Shaders_T,
	typename Uni_T,					// Class implementing setting uniforms and rendering them
	typename Subroutines_T = Subroutines<Shaders_T>>
class Program :
	protected ProgramBase<Uni_T>
{	//static_assert(std::is_base_of_v<FragComp_t, Shader<SFile>>, "Must have a proper fragment/compute shader type set");
	class LoadState;	//fwd decl
	LoadState load_state = LoadState(*this);

	using Comp_t = typename Shaders_T::Comp;
	using Frag_t = typename Shaders_T::Frag;
	using Vert_t = typename Shaders_T::Vert;
	using Geom_t = typename Shaders_T::Geom;
	using TesC_t = typename Shaders_T::TesC;
	using TesE_t = typename Shaders_T::TesE;

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
	Comp_t	comp;
	Frag_t	frag;	Vert_t vert;	Geom_t geom;	TesC_t tesc;	TesE_t tese;
	std::string program_name;
};

#include "Program.inl"