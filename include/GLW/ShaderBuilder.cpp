
#include "ShaderBuilder.h"

#include <fstream>
#include <regex>

namespace GLWRAPPER_NS {

	std::string ShaderBuilder::DirName(std::string path) {
		std::size_t found;
		found = path.find_last_of("/\\");
		return path.substr(0, found);
	}

	void ShaderBuilder::AppendLine(std::string line) {
		source += line + '\n';
	}

	void ShaderBuilder::AppendString(std::string text) {
		source += text;
	}

	void ShaderBuilder::AppendFile(std::string path, bool with_includes) {
		
		std::string dir = DirName(path);
		std::ifstream file(path);

		std::string result;

		std::regex reg_include("#include\\s+\"(.*)\"");
		std::smatch reg_match;

		std::string str;
		source += "#line 0 \"" + path + "\"\n";
		while (std::getline(file, str))
		{
			if (
				with_includes &&
				std::regex_match(str, reg_match, reg_include) &&
				reg_match.size() == 2
			) {
				AppendFile(dir + '/' + reg_match[1].str(), with_includes);
			} else {
				AppendLine(str);
			}
		}
	}

	void ShaderBuilder::Build() {

		ResourceBuilder::Build();

		ShaderData shader_data;

		// Generate shader id
		shader_data.shader_id = glCreateShader(shader_type);
		shader_data.shader_type = shader_type;

		// Check for error
		if (shader_data.shader_id == 0)
		{
			assert(false);
			return;
		}

		// Load shader from source string
		const char* source_prt = source.c_str();
		glShaderSource(shader_data.shader_id, 1, &source_prt, NULL);

		// Compile the loaded shader
		glCompileShader(shader_data.shader_id);

		// Check for error
		GLint result = GL_FALSE;
		int infoLogLength;

		glGetShaderiv(shader_data.shader_id, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shader_data.shader_id, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (result == GL_FALSE)
		{
			// Pring error message
			std::vector<char> error_message(infoLogLength);
			glGetShaderInfoLog(shader_data.shader_id, infoLogLength, NULL, error_message.data());

			fprintf(stdout, "%s\n", error_message.data());
			//assert(false);
			return;
		}

		object.Reset(std::move(shader_data));

	}

}