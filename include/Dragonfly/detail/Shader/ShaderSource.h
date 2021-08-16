#pragma once
// New!!
#include <string>
#include <variant>
#include <filesystem>
#include <vector>
#include <string_view>
#include "../FileIO/FileIO.h"

namespace df::detail{

inline FileCache ShaderFileCache;

enum class SHADER_TYPE
{
	NONE		= 0,
	VERTEX		= 1<<0,
	GEOMETRY	= 1<<1,
	TESS_CTRL	= 1<<2,
	TESS_EVAL	= 1<<3,
	FRAGMENT	= 1<<4,
	COMPUTE		= 1<<5,
}; // might be useful to have them as flags later

class ShaderSource;

struct GeneratedCode
{
public:
	struct ConcatenatedCode
	{
		struct SourceLocation{size_t index = 0; size_t line = 0;};
		std::string code;
		std::vector<size_t> lineNumbers;
		SourceLocation LocateLine(size_t line_) const;
	};
public:
	void PushBack(std::string_view str);
	void PushBack(const std::optional<std::string_view> &maybestr){ if(maybestr) PushBack(*maybestr);}

	void SetVersionNumber(int version_);
		
	const char** GetStrings() { return _strings.data(); }
	const int*   GetLengths() { return _lengths.data(); }
	size_t	GetCount() const {ASSERT(_strings.size() > 1 && _strings.size() == _lengths.size(), "Invalid source."); return _strings.size(); }

	ConcatenatedCode Concatenate() const;

	GeneratedCode(const ShaderSource& source_);
	GeneratedCode(const GeneratedCode&) = delete;
private:
	int _versionNumber; // Default GLSL version is OpenGL 3.0
	std::string _versionString;
	std::vector<const char*> _strings;
	std::vector<int>		 _lengths;
	size_t _totalLength = 0;
};


template<SHADER_TYPE ShaderType_>
class TypedShaderSource;

class ShaderSource
{
public:
	using OptionalStringView_t = std::optional<std::string_view>;
	struct FileSource {
		size_t hash;
		OptionalStringView_t GetSourceStr()const;
	};
	struct CodeSource {
		std::string code;
		OptionalStringView_t GetSourceStr() const { return static_cast<std::string_view>(code); }
	};
	using Source_Type = std::variant<FileSource, CodeSource>;
private:
	ShaderSource(std::vector<Source_Type> list_) : _list(std::move(list_)){}
public:
	ShaderSource(std::string code_) : _list({CodeSource{std::move(code_)}}) {}
	ShaderSource(std::filesystem::path path_) : _list({FileSource{ShaderFileCache.AddFile(std::move(path_))}}) {}
	template<SHADER_TYPE ShaderType_> ShaderSource(TypedShaderSource<ShaderType_> &&_other) : _list(std::move(_other._list)){}
public:
	//const GeneratedCode Generate() const;

	ShaderSource operator+(const ShaderSource& rh)&&;
	ShaderSource operator+(const ShaderSource& rh) const& { return ShaderSource(_list) + rh; }
	ShaderSource& operator+=(const ShaderSource& rh) {_list.insert(_list.end(),rh._list.begin(),rh._list.end()); return *this;}
	template<SHADER_TYPE ShaderType_>TypedShaderSource<ShaderType_> operator+(const TypedShaderSource<ShaderType_>& rh)  &&  { return TypedShaderSource<ShaderType_>(std::move(*this).operator+(static_cast<const ShaderSource&>(rh))); }
	template<SHADER_TYPE ShaderType_>TypedShaderSource<ShaderType_> operator+(const TypedShaderSource<ShaderType_>& rh)const&{ return TypedShaderSource<ShaderType_>(this->operator+(static_cast<const ShaderSource&>(rh))); }
	const std::vector<Source_Type>& GetList() const { return _list; }
protected:
	std::vector<Source_Type> _list;
};


template<SHADER_TYPE ShaderType_>
class TypedShaderSource : public ShaderSource
{
	using Base = ShaderSource;
public:
	TypedShaderSource(ShaderSource source_) : Base(std::move(source_)){}
	TypedShaderSource(std::string code_)				: TypedShaderSource(ShaderSource(code_)){}
	TypedShaderSource(std::filesystem::path path_)		: TypedShaderSource(ShaderSource(path_)){}
public:
	TypedShaderSource operator+ (const TypedShaderSource& rh)&&{return TypedShaderSource(Base::operator+(rh));}
	TypedShaderSource operator+ (const TypedShaderSource &rh)const&{return TypedShaderSource(Base::operator+(rh));}
	TypedShaderSource operator+ (const Base &rh)&&{return TypedShaderSource(Base::operator+(rh));}
	TypedShaderSource operator+ (const Base &rh)const&{return TypedShaderSource(Base::operator+(rh));}
	TypedShaderSource operator+=(const TypedShaderSource &rh){return TypedShaderSource(Base::operator+=(rh));}
	TypedShaderSource operator+=(const Base &rh){return TypedShaderSource(Base::operator+=(rh));}
};

using VertexShaderSource	= TypedShaderSource<SHADER_TYPE::VERTEX>;
using GeometryShaderSource	= TypedShaderSource<SHADER_TYPE::GEOMETRY>;
using TessCtrlShaderSource	= TypedShaderSource<SHADER_TYPE::TESS_CTRL>;
using TessEvalShaderSource	= TypedShaderSource<SHADER_TYPE::TESS_EVAL>;
using FragmentShaderSource	= TypedShaderSource<SHADER_TYPE::FRAGMENT>;
using ComputeShaderSource	= TypedShaderSource<SHADER_TYPE::COMPUTE>;

}//namespace df::detail

namespace df{
inline detail::ShaderSource Code(std::string code_) {
	return detail::ShaderSource(code_);
}
}//namespace df


inline df::detail::ShaderSource			operator"" _code0(const char* str, size_t len) { return df::detail::ShaderSource(std::string(str,len)); }
inline df::detail::ShaderSource			operator"" _file0(const char* str, size_t len) { return df::detail::ShaderSource(std::filesystem::path(str,str+len)); }
inline df::detail::ComputeShaderSource	operator"" _comp0(const char* str, size_t len) { return df::detail::ComputeShaderSource (std::filesystem::path(str,str+len)); }
inline df::detail::FragmentShaderSource operator"" _frag0(const char* str, size_t len) { return df::detail::FragmentShaderSource(std::filesystem::path(str,str+len)); }
inline df::detail::VertexShaderSource	operator"" _vert0(const char* str, size_t len) { return df::detail::VertexShaderSource  (std::filesystem::path(str,str+len)); }
inline df::detail::GeometryShaderSource operator"" _geom0(const char* str, size_t len) { return df::detail::GeometryShaderSource(std::filesystem::path(str,str+len)); }
inline df::detail::TessCtrlShaderSource operator"" _tesc0(const char* str, size_t len) { return df::detail::TessCtrlShaderSource(std::filesystem::path(str,str+len)); }
inline df::detail::TessEvalShaderSource operator"" _tese0(const char* str, size_t len) { return df::detail::TessEvalShaderSource(std::filesystem::path(str,str+len)); }


