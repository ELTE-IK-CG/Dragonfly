#pragma once

#include <cstdint>
#include <cassert>

#include <GL/glew.h>
//#include <SDL_opengl.h>

#include <glm/glm.hpp>

#define GLWRAPPER_NS gl

namespace GLWRAPPER_NS {
	using size_t = uint32_t;

	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	using vec4 = glm::vec4;
	using mat3 = glm::mat3;
	using mat4 = glm::mat4;
}