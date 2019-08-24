#include "Shader.h"
#include "ShaderBuilder.h"

namespace GLWRAPPER_NS {

	Shader Shader::FromFile(std::string path, GLenum shader_type) {

		gl::Shader shader;
		gl::ShaderBuilder builder(shader, shader_type);

		builder.AppendLine("#version 450");
		builder.AppendFile(path);
		builder.Build();

		return shader;

	}

}