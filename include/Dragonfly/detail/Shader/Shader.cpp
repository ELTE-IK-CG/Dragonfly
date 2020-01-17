#include "Shader.h"
#include "Shader.inl"
#include "../File/File.h"

using namespace df;

template class Shader<SFile>;

const char* shader2str(GLenum type){
	switch (type)	{
	case GL_COMPUTE_SHADER: return "comp";
	case GL_FRAGMENT_SHADER: return "frag";
	case GL_VERTEX_SHADER: return "vert";
	case GL_GEOMETRY_SHADER: return "geom";
	case GL_TESS_CONTROL_SHADER: return "tesc";
	case GL_TESS_EVALUATION_SHADER: return "tese";
	}
	ASSERT(false, "Should not get here."); return nullptr;
}

ShaderLowLevelBase::ShaderLowLevelBase(GLenum type) : shader_id(glCreateShader(type)), type(type), type_str(shader2str(type))
{
	ASSERT(type == GL_COMPUTE_SHADER || type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	GPU_ASSERT(glIsShader(shader_id), "Invalid shader");
}

ShaderLowLevelBase::~ShaderLowLevelBase()
{
	ASSERT(type == GL_COMPUTE_SHADER || type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	GPU_ASSERT(glIsShader(shader_id), "Invalid shader");
	glDeleteShader(shader_id);
}

bool ShaderLowLevelBase::Compile()
{
	ASSERT(type == GL_COMPUTE_SHADER || type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	ASSERT(source_strs.size() > 1 && source_strs.size() == source_lens.size(), "Invalid source.");
	GPU_ASSERT(glIsShader(shader_id), "Invalid shader");

	error_msg.clear();
	glShaderSource(shader_id, (GLsizei)source_strs.size(), source_strs.data(), source_lens.data());
	glCompileShader(shader_id);
	GLint result = 0, loglen = 0, errlen = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &loglen);
	if (result == GL_FALSE || loglen != 0) {
		std::vector<char> errmsg(loglen);
		glGetShaderInfoLog(shader_id, loglen, &errlen, errmsg.data());
		error_msg.assign(errmsg.data(), errmsg.data() + errlen);
	}
	return result;
}
