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

} //namespace df
