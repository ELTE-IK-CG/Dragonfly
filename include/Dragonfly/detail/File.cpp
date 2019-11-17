#include "File.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

using namespace df;

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
	getline(file, line); //first line may contain version info... no other line should
	if (line.rfind("#version ", 0) == 0)
	{
		version_number= std::stoi(line.substr(9, 4));
		ASSERT(100 <= version_number && version_number <= 460, ("Invalid GLSL version specified in " + path + ".").c_str());
	}
	else
	{
		code += line + '\n';
	}
	while (getline(file, line))
	{
		code += line + '\n';
	}
	file.close();	error_msg.clear();
	return true;
}

bool SFile::Save() const{
	std::ofstream out(path, std::ofstream::binary || std::ofstream::trunc);
	if(!out.is_open())	{
		error_msg += "Could not open file : " + path + '\n';
		return false;
	}
	if (version_number != 0)
	{
		std::string str = "#version " + std::to_string(version_number) + '\n';
		out.write(str.c_str(), str.length());
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