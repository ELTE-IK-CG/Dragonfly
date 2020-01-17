#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include "../../config.h"

//	1. Choose a file loader:		[DONE]
//		SFile,						Storage and interface between a file and memory copy
//		FileEditor					Includes a text editor as well
//			new implementations should inherit from SFile
//
//	2. Choose Shader class.			[DONE] They can take any file loader class as template parameter.
//		Shader<File_t=SFile>,		Encapsulates an OpenGL shader
//		ShaderEditor<File_t>		Allows editing how the shader is assembled and compiled
//			new implementations should inherit from Shader<File_t>
//
//	3. Choose a Program class:		[DONE]  TODO: explain

namespace df
{

template<typename File_t> class ShaderBase;

template<typename File_t>
class Shader : public ShaderBase<File_t>
{
	using Base = ShaderBase<File_t>;
	static_assert(std::is_base_of_v<SFile, File_t>,"File_t must be derived from SFile");
private:
	std::vector<std::string> extra_lines;
	friend class ProgramLowLevelBase;
protected:
	std::vector<File_t> shaders;
	Shader() = delete;
public:
	Shader(GLenum type);
	~Shader();

	//Push a shader file 
	Shader& operator <<(File_t &&sfile);
	Shader& operator <<(const std::string& path);

	//Delete a shader
	void PopShader();
	void EraseShader(size_t idx);

	//You can only read this data
	inline const File_t&	  GetShader(size_t idx) const { ASSERT(idx < shaders.size() && idx < 0, "Invalid index"); return shaders[idx]; }

	//Gathers source code from added shaders and compiles (does not "relaod" shaders)
	bool Compile();

	//This class doesn't (really) implement these features:
	void Render(std::string name = "") {}	void Update();
};

} //namespace df

#include "Shader.inl"
