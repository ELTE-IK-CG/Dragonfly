#pragma once
// New!!
#include "../../config.h"
#include <unordered_map>
#include <filesystem>
#include <optional>

namespace df::detail {

class FileIO {
public:
	using Path_Type = std::filesystem::path;
private:
	FileIO(const FileIO&) = delete;
	FileIO operator=(const FileIO&) = delete;
public:

	FileIO(const Path_Type &path) : _path(path), _dirtyBit(false) { Load(); }

	const std::string& GetContent() const { return _content; }
	void SetContent(const std::string& newContent) { _dirtyBit = true; _content = newContent; }
		
	bool Load();
	bool Save();

	Path_Type GetPath() const { return _path; }

private:
	const Path_Type _path;
	std::string _content;
	bool _dirtyBit = false; //https://www.youtube.com/watch?v=JwQZQygg3Lk
};

class FileCache
{
public:
	using Path_Type = std::filesystem::path;
	using FileIO_cptr = FileIO* const;
private:
	FileCache(const FileCache&) = delete;
	FileCache operator=(const FileCache&) = delete;
public:
	static Path_Type GetAbsolutePath(const Path_Type& path_);
	static size_t GetHashValue(const Path_Type& path_);

	FileCache() {}

	// If not in the cache already, the file is loaded. Hash is returned
	size_t AddFile(const Path_Type &path_);

	// Get file from cache. Use GetHashValue to compute hash_.
	FileIO_cptr GetFile(size_t hash_);

	// Get the path from a known hash value
	Path_Type GetPath(size_t hash_);

private:
	std::unordered_map<size_t, FileIO> _cache;
};

}//df::detail