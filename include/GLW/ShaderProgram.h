#pragma once

#include "ResourceOwner.h"
#include "Shader.h"
#include "Texture.h"

#include <unordered_map>
#include <vector>
#include <functional>

namespace GLWRAPPER_NS {

	struct ShaderProgramData {
		GLuint program_id = 0;
		std::vector<Shader> shaders;
		mutable std::unordered_map<std::string, GLint> locations;

		ShaderProgramData() {}
		ShaderProgramData(GLuint program_id) : program_id(program_id) {}

		void Reset() {
			if (program_id > 0) {
				glDeleteProgram(program_id);
			}
		}

		GLint GetLocation(std::string uniform) const {
			auto loc_it = locations.find(uniform);
			if (loc_it == locations.end()) {
				GLint my_loc = glGetUniformLocation(program_id, uniform.c_str());

				if (my_loc == -1) {
					//std::cout << "Uniform not found: " << uniform << std::endl;
				}

				locations[uniform] = my_loc;
				return my_loc;
			}
			else {
				return loc_it->second;
			}
		}
	};

	class ShaderProgram : public ResourceOwner<ShaderProgramData> {

	public:

		ShaderProgram() : ResourceOwner() {}

		void SetUniform(std::string uniform, const vec2& vec);
		void SetUniform(std::string uniform, const vec3& vec);
		void SetUniform(std::string uniform, const vec4& vec);
		void SetUniform(std::string uniform, const mat3& mat);
		void SetUniform(std::string uniform, const mat4& mat);
		void SetUniform(std::string uniform, int ival);
		void SetUniform(std::string uniform, float fval);


		void SetTexture(std::string uniform, GLenum target, GLuint textureID, int sampler);
		//void SetTexture2D(std::string uniform, GLuint textureID, int sampler);
		//void SetTexture3D(std::string uniform, GLuint textureID, int sampler);
		void SetTexture(std::string uniform, Texture& texture, int sampler);

		void Use() {
			glUseProgram(Get().program_id);
		}

		void Unuse() {
			glUseProgram(0);
		}

		void Use(std::function<void(ShaderProgram& program)> fnc) {
			Use();

			fnc(*this);

			Unuse();
		}

	};

}