#include "CompiledShader.h"
#include <regex>
#include <string_view>
#include <sstream>

using namespace df::detail;


CompiledShaderBase::CompiledShaderBase(detail::ShaderSource source_, GLenum type_, COMPILED_SHADER_FLAGS flags_)
	: _type(type_), _flags(flags_), _isCompiled(false),
	  _id(glCreateShader(type_)),   _source(std::move(source_))
{
	ASSERT(_type == GL_COMPUTE_SHADER || _type == GL_FRAGMENT_SHADER || _type == GL_VERTEX_SHADER || _type == GL_GEOMETRY_SHADER || _type == GL_TESS_CONTROL_SHADER || _type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	GPU_ASSERT(glIsShader(_id), "Invalid shader");
	_Compile();
}


CompiledShaderBase::VENDOR CompiledShaderBase::GetVendor()
{ 
	static VENDOR vendor = [](){
		auto str = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		bool nvidia = std::string_view(str, 6) == "NVIDIA";
		bool amd = std::string_view(str, 3) == "ATI" || std::string_view(str, 3) == "AMD"; 
		bool intel = std::string_view(str, 5) == "Intel";
		WARNING(int(nvidia) + int(amd) + int(intel) != 1, "Faild to recognise vendor.");
		return	nvidia ? VENDOR::NVIDIA : amd ? VENDOR::AMD : intel ? VENDOR::INTEL : VENDOR::UNKNOWN;
	}();
	return vendor;
}

CompiledShaderBase::~CompiledShaderBase()
{
	ASSERT(_type == GL_COMPUTE_SHADER || _type == GL_FRAGMENT_SHADER || _type == GL_VERTEX_SHADER || _type == GL_GEOMETRY_SHADER || _type == GL_TESS_CONTROL_SHADER || _type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	GPU_ASSERT(glIsShader(_id), "Invalid shader");
	if(_id != 0) glDeleteShader(_id);
}

CompiledShaderBase::Error CompiledShaderBase::Error::Parse(const std::string &line_)
{
	// TODO info/message level?
	static const auto matcher_nvidia = [](const std::string& line) ->Error {
		static std::regex r = std::regex(R"((.*?)\((\d+)\)\s*:\s*(error|warning)\s*C(\d{4}):\s*(.*?)\s*)", std::regex::flag_type::optimize);
		std::smatch m; Error ret;
		if (std::regex_match(line, m, r)) {
			/*ret.path = m[1];*/
			ret.absPosition = std::stoi(m[2]);
			ret.type =
				m[3] == "error" ?	Error::TYPE::ERROR :
				m[3] == "warning" ?	Error::TYPE::WARNING : 
									Error::TYPE::UNKNOWN;
			/*ret.e_code = std::stoi(m[4]);*/
			ret.content = m[5];
		}
		else {
			ret.content = line;
		}
		return ret;
	};
	// TODO info/message level?
	static const auto matcher_amd = [](const std::string& line) ->Error {
		static std::regex r = std::regex(R"((ERROR|WARNING):\s*(\d+):(\d+):\s*(.*?)\s*)", std::regex::flag_type::optimize);
		std::smatch m; Error ret;
		if (std::regex_match(line, m, r)) {
			ret.type =
				m[1] == "ERROR" ?	Error::TYPE::ERROR :
				m[1] == "WARNING" ?	Error::TYPE::WARNING :
									Error::TYPE::UNKNOWN;
			ret.sourceLoc.index = std::stoi(m[2]);
			ret.sourceLoc.line = std::stoi(m[3]);
			ret.content = m[4];
		} else {
			ret.content = line;
		}
		return ret;
	};
	// TODO info/message level?
	static const auto matcher_intel = [](const std::string& line) ->Error {
		static std::regex r = std::regex(R"((ERROR|WARNING):\s*(\d+):(\d+):\s*(.*?)\s*)", std::regex::flag_type::optimize);
		std::smatch m; Error ret;
		if (std::regex_match(line, m, r)) {
			ret.type =
				m[1] == "ERROR" ? Error::TYPE::ERROR :
				m[1] == "WARNING" ? Error::TYPE::WARNING :
				Error::TYPE::UNKNOWN;
			ret.sourceLoc.index = std::stoi(m[2]);
			ret.sourceLoc.line = std::stoi(m[3]) - 1; //todo check on another intel if this is the case
			ret.content = m[4];
		} else {
			ret.content = line;
		}
		return ret;

	};

	static auto& def = matcher_intel;
	auto vendor = GetVendor();
	return	vendor == VENDOR::NVIDIA? matcher_nvidia(line_) :
			vendor == VENDOR::AMD	? matcher_amd(line_)	:
			vendor == VENDOR::INTEL ? matcher_intel(line_)	: def(line_);
}

void CompiledShaderBase::_ErrorHandling(const GeneratedCode &code_)
{
	auto gen = code_.Concatenate();
	std::istringstream ss(_messages);
	std::string line;
	while (std::getline(ss, line)) {
		if (line.empty()) continue;
		auto err = Error::Parse(line);
		if (err.type != Error::TYPE::UNKNOWN) {
			if (GetVendor() == VENDOR::NVIDIA) {
				err.sourceLoc = gen.LocateLine(err.absPosition);
			}
			else {
				err.absPosition = gen.lineNumbers[err.sourceLoc.index] + err.sourceLoc.line;
			}
		}
		const auto& src = _source.GetList()[err.sourceLoc.index-1];
		if (std::holds_alternative<ShaderSource::FileSource>(src))
		{
			auto absPath = ShaderFileCache.GetPath(std::get< ShaderSource::FileSource>(src).hash);
			auto absPathS = absPath.string();
			auto relPathS = absPath.lexically_proximate(FileCache::GetAbsolutePath(".")).string();
			err.path = std::move(absPathS.length() < relPathS.length() ? absPathS : relPathS);
		}
		else
			err.path = "[generated code]";
		_errors.emplace_back(std::move(err));
	}
	if(_flags && COMPILED_SHADER_FLAGS::SAVE_GENERATED_CODE)
		_generatedCode = std::move(gen);
}

bool CompiledShaderBase::_Compile()
{
	ASSERT(_type == GL_COMPUTE_SHADER || _type == GL_FRAGMENT_SHADER || _type == GL_VERTEX_SHADER || _type == GL_GEOMETRY_SHADER || _type == GL_TESS_CONTROL_SHADER || _type == GL_TESS_EVALUATION_SHADER, "Invalid shader type");
	GPU_ASSERT(glIsShader(_id), "Invalid shader");
	
	_messages.clear();

	auto code = GeneratedCode(_source);

	glShaderSource(_id, code.GetCount(), code.GetStrings(), code.GetLengths());
	glCompileShader(_id);

	GLint result = 0, loglen = 0, errlen = 0;
	glGetShaderiv(_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &loglen);
	
	if (result == GL_FALSE || loglen != 0) {
		std::vector<char> errmsg(loglen);
		glGetShaderInfoLog(_id, loglen, &errlen, errmsg.data());
		_messages.assign(errmsg.data(), errmsg.data() + errlen);
		if(result == GL_FALSE || (_flags && COMPILED_SHADER_FLAGS::ALLOW_SHADER_WARNINGS))
			_ErrorHandling(code);
	}
	_isCompiled = result == GL_TRUE;
	return _isCompiled;
}

std::ostream& df::detail::operator<<(std::ostream& s, const CompiledShaderBase::Error& err)
{
	using E = CompiledShaderBase::Error::TYPE;
	s << (err.type == E::ERROR ? "[ ERROR ]" : err.type == E::WARNING ? "[WARNING]" : "[MESSAGE]");
	if (err.type == E::UNKNOWN)
		s << " \"" << err.content << '\"';
	else
		s << ' ' << err.content << "  at line " << err.sourceLoc.line << " in \"" << err.path << "\" (" << err.absPosition << ')';
	return s;
}
