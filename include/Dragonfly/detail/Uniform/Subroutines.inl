#pragma once

#include "Subroutines.h"

namespace df
{

struct NoShader; // definition in Program.inl 

template<typename Shaders_T>
Subroutines<Shaders_T>::Subroutines(GLint program)
{
	if constexpr (!std::is_same_v<typename Shaders_T::Comp, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_COMPUTE_SHADER));
	}
	if constexpr (!std::is_same_v<typename Shaders_T::Frag, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_FRAGMENT_SHADER));
	}
	if constexpr (!std::is_same_v<typename Shaders_T::Vert, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_VERTEX_SHADER));
	}
	if constexpr (!std::is_same_v<typename Shaders_T::Geom, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_GEOMETRY_SHADER));
	}
	if constexpr (!std::is_same_v<typename Shaders_T::TesC, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_TESS_CONTROL_SHADER));
	}
	if constexpr (!std::is_same_v<typename Shaders_T::TesE, NoShader>) {
		shaderSubs.emplace_back(ShaderSubroutines(program, GL_TESS_EVALUATION_SHADER));
	}
}

} // namespace df