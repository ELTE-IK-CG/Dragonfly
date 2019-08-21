#include "Shader.h"
#include <filesystem>
#include <fstream>

// ========================= File ==============================

SFile::SFile(const std::string &path_){
	SetLocation(path_);
	Load();
}

void SFile::SetLocation(const std::string &path_){
	path = path_;
	for (auto &p : std::filesystem::path(path))	{
		folder = filename;
		filename = p.generic_string();
		++folder_depth_level;
	}
	extension = std::filesystem::path(filename).extension().generic_string();
	filename = std::filesystem::path(filename).replace_extension().generic_string();
	//line0 = "#line 0 \"" + path + "\"\n";
}

bool SFile::Load(){
	std::ifstream file(path);
	code.clear();
	if(!file.is_open())	{
		error_msg += "Could not open file : " + path + '\n';
		return false;
	}
	std::string line;
	while(getline(file, line))
		code += line + '\n';
	file.close();	error_msg.clear();
	return true;
}

bool SFile::Save() const{
	std::ofstream out(path, std::ofstream::binary || std::ofstream::trunc);
	if(!out.is_open())	{
		error_msg += "Could not open file : " + path + '\n';
		return false;
	}
	out.write(code.c_str(), code.length());
	out.close();	error_msg.clear();
	return true;
}

void SFile::Assign(const std::string & code_){
	code = code_; dirty = true;
	path = folder = filename = extension = error_msg = "";
	folder_depth_level = 0;
}
// ========================= Shader ==============================

#include "Shader.inl"

const char* shader2str(GLenum type){
	switch (type)	{
	case GL_COMPUTE_SHADER: return "comp"; break;
	case GL_FRAGMENT_SHADER: return "frag"; break;
	case GL_VERTEX_SHADER: return "vert"; break;
	case GL_GEOMETRY_SHADER: return "geom"; break;
	case GL_TESS_CONTROL_SHADER: return "tesc"; break;
	case GL_TESS_EVALUATION_SHADER: return "tese"; break;
	default: ASSERT(true, "Should not get here."); return nullptr; break;
	}
	ASSERT(true, "Should not get here."); return nullptr;
}

template class Shader<SFile>;

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
