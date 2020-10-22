#include "ShaderSource.h"
#include <iostream>

using namespace df::detail;

GeneratedCode::GeneratedCode(const ShaderSource& source_)
{
	const size_t defaultVersion = 130;
	const auto& list = source_.GetList();
	_strings.reserve(list.size() + 1);	_strings.emplace_back(""); //so size == 1
	_lengths.reserve(list.size() + 1);	_lengths.emplace_back(0);
	SetVersionNumber(defaultVersion);
	for (auto& v : list)
		PushBack(std::visit([](auto&& x)->ShaderSource::OptionalStringView_t { return x.GetSourceStr(); }, v));

}

ShaderSource ShaderSource::operator+(const df::detail::ShaderSource& rh)&& {
	ShaderSource new_source(std::move(_list));
	new_source._list.insert(new_source._list.end(), rh._list.begin(), rh._list.end());
	return new_source;
}

ShaderSource::OptionalStringView_t ShaderSource::FileSource::GetSourceStr() const {
	if (auto opt = ShaderFileCache.GetFile(hash))
		return static_cast<std::string_view>(opt->GetContent());
	else
		return std::nullopt;
}

inline void GeneratedCode::PushBack(std::string_view str)
{
	int ver = 0;
	//todo glsl correct version recognition
	if (str.substr(0, 9) == "#version "){
		ver = std::atoi(str.substr(9,12).data());
		str = str.substr(12);
	}
	else if (str.substr(0, 12) == "//?#version "){
		ver = std::atoi(str.substr(12,15).data());
		str = str.substr(15);
	}

	if (ver > _versionNumber) SetVersionNumber(ver);

	_strings.emplace_back(str.data());
	_lengths.emplace_back(str.length());
	_totalLength += str.length();
}

void GeneratedCode::SetVersionNumber(int version_)
{
	_totalLength -= _lengths[0];
	ASSERT(100 <= version_ && version_ <= 460, ("Invalid GLSL version number " + std::to_string(version_)).c_str());
	_versionNumber = version_;
	_versionString = "#version " + std::to_string(_versionNumber) + '\n';
	_strings[0] = _versionString.data();
	_lengths[0] = _versionString.length();
	_totalLength += _lengths[0];
}

GeneratedCode::ConcatenatedCode::SourceLocation GeneratedCode::ConcatenatedCode::LocateLine(size_t line_) const
{
	SourceLocation ret;
	if (line_ > lineNumbers.back()) {
		WARNING(true, "lineNumbers_ is too large.");
		ret.index = lineNumbers.size() - 2;
		ret.line = lineNumbers.back() - lineNumbers[lineNumbers.size() - 2];
	}
	else {
		auto it = std::upper_bound(lineNumbers.cbegin(), lineNumbers.cend(), line_) - 1;
		ASSERT(it + 1 == lineNumbers.cend() || line_ <= *(it + 1), "Error finding exact line of the shader error message in file.");
		ASSERT(0 <= *it && *it <= line_, "Error finding exact line of the shader error message in file.");
		ret.line = line_ - *it + 1; // todo check
		ret.index = std::distance(lineNumbers.cbegin(), it);
		ASSERT(0 <= ret.index && ret.index < lineNumbers.size() - 1, "Error finding exact line of the shader error message in file.");
	}
	return ret;
}

GeneratedCode::ConcatenatedCode GeneratedCode::Concatenate() const {
	ConcatenatedCode ret;
	ret.code.reserve(_totalLength + GetCount() + 1);
	ret.lineNumbers.resize(GetCount() + 1);
	size_t lineCount = 0;
	ret.lineNumbers[0] = lineCount;
	for (size_t i = 0; i < GetCount(); ++i)
	{
		lineCount += 1 + std::count(_strings[i], _strings[i] + _lengths[i], '\n');
		ret.lineNumbers[i + 1] = lineCount;
		ret.code.append(_strings[i], _lengths[i]);
		ret.code += '\n';
	}
	return ret;
}
