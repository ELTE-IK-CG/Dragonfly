#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include "../config.h"

//	1. Choose a file loader:		[DONE]
//		SFile,						Storage and interface between a file and memory copy
//		SFileEditor					Includes a text editor as well
//			new implementations should inherit from SFile
//
//	2. Choose Shader class.			[WIP] They can take any file loader class as template parameter.
//		Shader<File_t=SFile>,		Encapsulates an OpenGL shader
//		ShaderEditor<File_t>		Allows editing how the shader is assembled and compiled
//			new implementations should inherit from Shader<File_t>
//
//
//	3. Choose a Program class:		[TODO] Not yet designed
//		Program<Vert_t/Comp_t,..., Frag_t>,				No UI functionality (not variadic template, just one for each shader)
//		ProgramEditor<Vert_t/Comp_t,..., Frag_t>		Can compile & link shaders.
//		ProgramDebug<Vert_t/Comp_t,..., Frag_t>			Uniforms are captured and displayed
//		ProgramDebugEditor<...>							All the above (diamond problem?)
//		ProgramAnimate<...>								Uniforms can be set as a function of time.(ProgramDebug+animation)
//		ProgramAnimateEditor<...>						All the above (crystal problem? :D)
//			new implementations should inherit from Program<...>
//		
//		Alternatively have a Uniform, UniformDebug, UniformAnimate classes Program[Editor]<Uniform,Vert...>
//		Problem is that a standalone Uniform class makes no sense. (UBO-s are different)


class SFile {
private:
	void SetLocation(const std::string &path_);
protected:
	std::string path;
	std::string folder, filename, extension;
	std::string code;
	int version_number = 130; // default version number (130)
	int folder_depth_level = 0;
	mutable std::string error_msg;
	mutable bool dirty = false;
	SFile() = delete;
	SFile(SFile &) = delete;
	SFile& operator=(const SFile&) = delete;
public:
	SFile(SFile &&) = default;
	SFile& operator=(SFile&&) = default;
	//Create and Load shader file assosiated with path_
	SFile(const std::string &path_);

	//Reload or Save file
	bool Load();
	bool Save() const;

	//Overwrite code
	void Assign(const std::string &code_);

	//Append code
	inline void Append(const std::string &code_) { code += code_; dirty = true; }

	//You cannot change
	inline const std::string& GetPath() const { return path; }
	inline const std::string& GetFolder() const { return folder; }
	inline const std::string& GetFilename() const { return filename; }
	inline const std::string& GetExtension() const { return extension; }
	inline const std::string& GetCode() const { return code; }
	inline const std::string& GetErrors() const { return error_msg; }
	inline const int GetVersionNumber() const { return version_number; }
	// True if content of file differs from code in memory
	inline bool isDirty() const { return dirty; }

	//This class doesn't implement these features:
	void Render(std::string name = "") {} void Update() {} void Open() {} void Close() {} void ViewFile(){}
};

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

#include "Shader.inl"
