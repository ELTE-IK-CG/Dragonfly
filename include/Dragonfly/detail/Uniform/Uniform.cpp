#include "Uniform.h"

using namespace df;

bool Uniforms::Compile()
{
	GLint uni_num = 0, uni_max_name_len = 0;
	GPU_ASSERT(program_id != 0 && glIsProgram(program_id), "Invalid shader program");
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &uni_num);
	WARNING(uni_num == 0, "The shader program does not have any uniforms");
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uni_max_name_len);
	ASSERT(uni_num == 0 || uni_max_name_len > 1, "Weird. The uniform names should be longer.");
	std::vector<char> namebuff(uni_max_name_len);
	locations.clear();
	sampler2texLoc.clear();
	texLoc2sampler.clear();
	locations.reserve(uni_num);
	for (int i = 0; i < uni_num; ++i)	{
		GLsizei size; GLenum type;
		glGetActiveUniform(program_id, (GLuint)i, (GLsizei)uni_max_name_len, nullptr, &size, &type,	namebuff.data());
		Values vals;
#ifdef _DEBUG
		vals.size = size; vals.gpu_type = type;
#endif // _DEBUG
		vals.loc = static_cast<uint16_t>(glGetUniformLocation(program_id, namebuff.data()));
		if (isOpenGLTextureType(type))
		{
			sampler2texLoc.emplace_back(vals.loc);
			texLoc2sampler.emplace(vals.loc, static_cast<uint8_t>(sampler2texLoc.size()-1));
		}
		locations.emplace(namebuff.data(), vals);
	}
	locations.rehash(0);
	return true;
}
