#pragma once

#include <string>
#include <variant>
#include <filesystem>
#include <vector>
#include "../FileIO/FileIO.h"

namespace df{namespace detail{

static df::detail::FileCache ShaderFileCache;

class ShaderSource
{
private:
	struct FileSource {
		size_t hash;
	};
	struct CodeSource {
		std::string code;
	};
	using Source_Type = std::variant<FileSource, CodeSource>;
private:
	ShaderSource(std::vector<Source_Type> list_) : _list(std::move(list_)){}
	ShaderSource(std::string code_) : _list({CodeSource{std::move(code_)}}) {}
	ShaderSource(std::filesystem::path path_) : _list({FileSource{ShaderFileCache.AddFile(std::move(path_))}}) {}
public:
	ShaderSource operator+(const ShaderSource& rh) && {
		ShaderSource new_source(std::move(_list));
		new_source._list.insert(new_source._list.end(), rh._list.begin(), rh._list.end());
		return new_source;
	}
	ShaderSource operator+(const ShaderSource& rh) const& { return ShaderSource(_list) + rh; }
	ShaderSource& operator+=(const ShaderSource& rh) {_list.insert(_list.end(),rh._list.begin(),rh._list.end()); return *this;}

private:
	std::vector<Source_Type> _list;
};

enum class SHADER_TYPE
{
	NONE		= 0,
	VERTEX		= 1<<0,
	GEOMETRY	= 1<<1,
	TESS_CTRL	= 1<<2,
	TESS_EVAL	= 1<<3,
	FRAGMENT	= 1<<4,
	COMPUTE		= 1<<5, //or this could be zero instead
}; // might be useful to have them as flags later


template<SHADER_TYPE ShaderType_>
class TypedShaderSource : private ShaderSource
{
	using Base = ShaderSource;
private:
	TypedShaderSource(ShaderSource source_) :Base(std::move(source_)){}
	TypedShaderSource(std::vector<Source_Type> list_)	: TypedShaderSource(ShaderSource(list_)){}
	TypedShaderSource(std::string code_)				: TypedShaderSource(ShaderSource(code_)){}
	TypedShaderSource(std::filesystem::path path_)		: TypedShaderSource(ShaderSource(code_)){}
public:
	TypedShaderSource operator+ (const TypedShaderSource& rh)&& {
		return TypedShaderSource(Base::operator+(reinterpret_cast<Base>(rh)));
	}
	TypedShaderSource operator+ (const TypedShaderSource &rh)const&{}
	TypedShaderSource operator+ (const Base &rh) && {}
	TypedShaderSource operator+ (const Base &rh)const&{}
	TypedShaderSource operator+=(const TypedShaderSource &rh){}
	TypedShaderSource operator+=(const Base &rh){}
};

using VertexShaderSource	= TypedShaderSource<SHADER_TYPE::VERTEX>;
using GeometryShaderSource	= TypedShaderSource<SHADER_TYPE::GEOMETRY>;
using TessCtrlShaderSource	= TypedShaderSource<SHADER_TYPE::TESS_CTRL>;
using TessEvalShaderSource	= TypedShaderSource<SHADER_TYPE::TESS_EVAL>;
using FragmentShaderSource	= TypedShaderSource<SHADER_TYPE::FRAGMENT>;
using ComputeShaderSource	= TypedShaderSource<SHADER_TYPE::COMPUTE>;

}}//namespace df::detail