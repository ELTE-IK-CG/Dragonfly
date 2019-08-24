#pragma once

#include "SharedResourceOwner.h"

#include <string>

namespace GLWRAPPER_NS {

	struct ShaderData {
		GLuint shader_id = 0;
		GLenum shader_type = GL_INVALID_ENUM;

		ShaderData() {}
		ShaderData(GLuint shader_id, GLenum shader_type) : shader_id(shader_id), shader_type(shader_type) {}

		void Reset() {
			if (shader_id > 0) {
				glDeleteShader(shader_id);
			}
		}
	};

	class Shader : public SharedResourceOwner<ShaderData> {

	public:

		Shader() : SharedResourceOwner() {}

		static Shader FromFile(std::string path, GLenum shader_type);

	};

}