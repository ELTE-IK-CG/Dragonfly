#include "FileIO.h"
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace df;

bool detail::FileIO::Load()
{
	std::ifstream file(_path);
	_content.clear();
	if (!file.is_open()) {
		WARNING(true, ("Could not open file " + _path.string() + " for reading").c_str());
		return false;
	}
	for (std::string line; std::getline(file, line);)
		_content += line + '\n';
	file.close();
	_dirtyBit = false;
	return true;
}

bool detail::FileIO::Save() 
{
	std::ofstream out(_path, std::ofstream::binary || std::ofstream::trunc);
	if (!out.is_open()) {
		WARNING(true, ("Could not open file " + _path.string() + " for writing").c_str());
		return false;
	}
	out.write(_content.c_str(), _content.length());
	out.close();
	_dirtyBit = false;
	return true;
}

detail::FileCache::Path_Type detail::FileCache::GetAbsolutePath(const Path_Type &path_) {
	std::error_code ec;
	auto apath = std::filesystem::absolute(path_, ec);
	ASSERT(!ec, ("Error converting " + path_.string() + " to absolute path").c_str());
	return apath;
}
size_t detail::FileCache::GetHashValue(const Path_Type &path_) {
	return std::filesystem::hash_value(GetAbsolutePath(path_));
}

size_t detail::FileCache::AddFile(const Path_Type &path_)
{
	size_t id = GetHashValue(path_);
	auto it = _cache.find(id);
	if (it == _cache.end()) {
		it = _cache.emplace(id, path_).first;
		//emplace().second guaranteed to be true since it == cache.end()
	}
	//return it->second;
	return id;
}

detail::FileCache::FileIO_cptr  detail::FileCache::GetFile(size_t hash_)	
{
	auto it = _cache.find(hash_);
	if(it == _cache.end()) return nullptr;
	return &it->second;
}