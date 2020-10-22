#pragma once
#include "ShaderSource.h"
#include <GL/glew.h>

namespace df{
	enum class COMPILED_SHADER_FLAGS
	{
		NONE = 0,
		ALLOW_SHADER_WARNINGS = 1,
		SAVE_GENERATED_CODE = 2,
		DEFAULT = NONE,
	}; ENUM_CLASS_FLAG_OPERATORS(COMPILED_SHADER_FLAGS);
}

namespace df::detail {
class CompiledShaderBase
{
public:
	struct Error {
		enum class TYPE {UNKNOWN, ERROR, WARNING, MESSAGE } type = TYPE::UNKNOWN;
		GeneratedCode::ConcatenatedCode::SourceLocation sourceLoc;
		struct SourceData
		{
			enum class TYPE{FILE, CODE} type;
			size_t id;
			std::string path;
		};
		int line = 0, col = 0;
		std::string content;
		static Error Parse(const std::string &line);
	};

	enum class VENDOR {UNKNOWN, NVIDIA, AMD, INTEL };
private:

	CompiledShaderBase() = delete;
protected:
	CompiledShaderBase(detail::ShaderSource source_, GLenum type_, COMPILED_SHADER_FLAGS flags_);
	~CompiledShaderBase();
	GLuint getID() const { return _id; }
	GLenum getType() const { return _type; }

	bool _Compile();
	void _ErrorHandling(const GeneratedCode& code_);

public:
	const std::string& GetMessages() const { return _messages; }
	const std::vector<Error>& GetErrors() const { return _errors; }
	const GeneratedCode::ConcatenatedCode& GetGeneratedSource() const { return _generatedCode; }
	bool IsCompiled() const { return _isCompiled; }
	explicit operator bool() const { return IsCompiled(); }
	static VENDOR GetVendor();

private:
	GLenum _type = 0;
	COMPILED_SHADER_FLAGS _flags;
	GLuint _id = 0;
	bool _isCompiled = false;
	detail::ShaderSource _source;
	std::vector<Error> _errors;
	std::string _messages;
	GeneratedCode::ConcatenatedCode _generatedCode;
	static VENDOR vendor_;
};

inline GLenum ConvertToGLenum(SHADER_TYPE type_) {
	switch(type_){
	case SHADER_TYPE::COMPUTE:   return GL_COMPUTE_SHADER;
	case SHADER_TYPE::VERTEX:    return GL_VERTEX_SHADER;
	case SHADER_TYPE::GEOMETRY:  return GL_GEOMETRY_SHADER;
	case SHADER_TYPE::TESS_CTRL: return GL_TESS_CONTROL_SHADER;
	case SHADER_TYPE::TESS_EVAL: return GL_TESS_EVALUATION_SHADER;
	case SHADER_TYPE::FRAGMENT:  return GL_FRAGMENT_SHADER;
	}
}

} //namespace df::detail


namespace df{

template<detail::SHADER_TYPE ShaderType_>
class CompiledShader : public detail::CompiledShaderBase
{
	using Base = detail::CompiledShaderBase;
public:
	CompiledShader(detail::TypedShaderSource<ShaderType_> source_, COMPILED_SHADER_FLAGS flags_ = COMPILED_SHADER_FLAGS::DEFAULT)
		: Base(detail::ShaderSource(std::move(source_)), ConvertToGLenum(ShaderType_), flags_){}
};

using CompiledVertexShader   = CompiledShader<detail::SHADER_TYPE::VERTEX>;
using CompiledGeometryShader = CompiledShader<detail::SHADER_TYPE::GEOMETRY>;
using CompiledTessCtrlShader = CompiledShader<detail::SHADER_TYPE::TESS_CTRL>;
using CompiledTessEvalShader = CompiledShader<detail::SHADER_TYPE::TESS_EVAL>;
using CompiledFragmentShader = CompiledShader<detail::SHADER_TYPE::FRAGMENT>;
using CompiledComputeShader  = CompiledShader<detail::SHADER_TYPE::COMPUTE>;

} //namespace df