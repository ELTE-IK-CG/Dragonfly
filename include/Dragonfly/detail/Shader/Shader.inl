#pragma once
#include <type_traits>
#include "Shader.h"

namespace df
{

//inline const char* shader2str(GLenum type);

#undef max

class ShaderLowLevelBase
{
private:
	const GLuint shader_id = 0;
	const GLenum type = 0;
	const std::string type_str;
	std::string error_msg;

	ShaderLowLevelBase() = delete;
protected:
	std::vector<GLint>		 source_lens;	//For efficient compilation
	std::vector<const char*> source_strs;	//For efficient compilation
	std::string				 version_str;

	ShaderLowLevelBase(GLenum type);
	~ShaderLowLevelBase();
	inline GLuint getID() const { return shader_id; }
	inline GLenum getType() const { return type; }
	inline const std::string& getTypeStr() const { return type_str; }

	bool Compile();
public:
	inline const std::string& GetErrors() const { return error_msg; }
};

template<typename File_t>
class ShaderBase : public ShaderLowLevelBase{
private:
	ShaderBase() = delete;
protected:
	ShaderBase(GLenum type) : ShaderLowLevelBase::ShaderLowLevelBase(type){}
}; //this means we can have the low level class declaration after the Shader class

} //namespace df

template<typename File_t> df::Shader<File_t>::Shader(GLenum type) : df::ShaderBase<File_t>::ShaderBase(type){}
template<typename File_t> df::Shader<File_t>::~Shader(){}

template<typename File_t>
bool df::Shader<File_t>::Compile(){
	this->source_strs.resize(2*shaders.size() + 1);
	this->source_lens.resize(2*shaders.size() + 1);
	extra_lines.resize(shaders.size());
	int ver_num = 110;		//smallest possible version number
	for (size_t i = 0; i < shaders.size(); ++i)	{
		extra_lines[i] = "/*************************************************\n" + shaders[i].GetPath() + "\n*************************************************/\n";
		const std::string &line0 = extra_lines[i];
		this->source_strs[2 * i + 1] = line0.c_str();
		this->source_lens[2 * i + 1] = (GLint)line0.length();

		const std::string &code = shaders[i].GetCode();
		this->source_strs[2 * i + 2] = code.c_str();
		this->source_lens[2 * i + 2] = (GLint)code.length();
		ver_num = ver_num > this->shaders[i].GetVersionNumber() ? ver_num : this->shaders[i].GetVersionNumber();
	}
	this->version_str = "#version " + std::to_string(ver_num) + '\n';
	this->source_strs[0] = this->version_str.c_str();
	this->source_lens[0] = (GLint)this->version_str.length();
	return ShaderLowLevelBase::Compile();
}

template<typename File_t>
df::Shader<File_t>& df::Shader<File_t>::operator <<(File_t &&sfile){
	shaders.emplace_back(std::move(sfile));
	return *this;
}
template<typename File_t>
df::Shader<File_t>& df::Shader<File_t>::operator <<(const std::string& path){
	shaders.emplace_back(path);
	return *this;
}

template<typename File_t>
void df::Shader<File_t>::PopShader() {
	shaders.pop_back();
}
template<typename File_t>
void df::Shader<File_t>::EraseShader(size_t idx) {
	ASSERT(idx < shaders.size() && idx >= 0,"Invalid index");	shaders.erase(shaders.begin() + idx);
}

template<typename File_t> void df::Shader<File_t>::Update() {
	for (auto& s : this->shaders) s.Update();
}