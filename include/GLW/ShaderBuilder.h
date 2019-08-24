#pragma once

#include "Shader.h"
#include "ResourceBuilder.h"

#include <string>

namespace GLWRAPPER_NS {

	class ShaderBuilder: public ResourceBuilder<Shader> {

		GLenum shader_type;

		std::string source;

	public:

		ShaderBuilder(Shader& shader, GLenum shader_type) : ResourceBuilder(shader), shader_type(shader_type) {}

		void AppendLine(std::string line);
		void AppendString(std::string text);
		void AppendFile(std::string path, bool with_includes = true);

		void Build() override;

	private:

		std::string DirName(std::string path);

	};

}