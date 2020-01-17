#pragma once
#include "../Shader/ShaderFwd.h"

namespace df
{
	template<typename Shaders_T> class Subroutines;
	template<typename Shaders_T, typename Uni_T, typename Subroutines_T=Subroutines<Shaders_T>> class Program;

	class Uniforms;

	using ShaderProgramVF = Program<ShaderVF, Uniforms>;
	using ShaderProgramVGF = Program<ShaderVGF, Uniforms>;
	using ShaderProgramVTF = Program<ShaderVTF, Uniforms>;
	using ShaderProgramVGTF = Program<ShaderVGTF, Uniforms>;
	using ComputeProgram = Program<ShaderCompute, Uniforms>;

	class ProgramLowLevelBase;
	template<typename Uniform_T> class ProgramBase;

	namespace detail
	{
		struct _CompShader { const char* path; };
		struct _FragShader { const char* path; };
		struct _VertShader { const char* path; };
		struct _GeomShader { const char* path; };
		struct _TescShader { const char* path; };
		struct _TeseShader { const char* path; };
	}

} //namespace df

