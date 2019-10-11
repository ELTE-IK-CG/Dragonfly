#include "Uniforms.h"

GLuint LazyUniforms::GetUniformLocation(const std::string & str) const {
	if (auto it = locations.find(str); it != locations.end()) {
		return it->second;
	}
	else if (const GLint uloc = glGetUniformLocation(program_id, str.c_str()); uloc != -1) {
		return locations[str] = uloc;
	}
	else {
		ASSERT(false, ("Uniform \"" + str + "\" is not an active uniform in the shader program.").c_str());
		return -1;
	}
}

GLuint GreedyUniforms::GetUniformLocation(const std::string& str) const {
	if (auto it = locations.find(str); it != locations.end()) {
		return it->second.loc;
	}
	else {
		WARNING(true, ("Uniform \"" + str + "\" location not found. The shader program might not have this uniform, or it was not properly compiled.").c_str());
		return -1;
	}
}

bool GreedyUniforms::Compile()
{
	GLint uni_num = 0, uni_max_name_len = 0;
	GPU_ASSERT(program_id != 0 && glIsProgram(program_id), "Invalid shader program");
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &uni_num);
	WARNING(uni_num == 0, "The shader program does not have any uniforms");
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uni_max_name_len);
	ASSERT(uni_num == 0 || uni_max_name_len > 1, "Weird. The uniform names should be longer.");
	std::vector<char> namebuff(uni_max_name_len);
	locations.reserve(uni_num);
	for (int i = 0; i < uni_num; ++i)	{
		GLsizei size; GLenum type;
		glGetActiveUniform(program_id, (GLuint)i, (GLsizei)uni_max_name_len, nullptr, &size, &type,	namebuff.data());
		Values vals;
#ifdef _DEBUG
		vals.size = size; vals.gpu_type = type;
#endif // _DEBUG
		vals.loc = glGetUniformLocation(program_id, namebuff.data());
		locations.emplace(namebuff.data(), vals);
	}
	locations.rehash(0);
	return true;
}
