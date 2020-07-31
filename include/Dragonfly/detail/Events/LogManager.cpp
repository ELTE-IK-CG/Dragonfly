#include "LogManager.h"

uint64_t df::LogManager::HashEntry(detail::Logger::Entry& entry) const
{
	uint64_t h1 = std::hash<std::string>{}(entry.message);
	uint64_t h2 = std::hash<std::string_view>{}(entry.location.filePath);
	uint64_t h3 = entry.location.line;

	detail::CombineHashes(h1, h2, h3);

	return h1;
}

void df::LogManager::SetFilterCondition(CONDITION_TYPE type, detail::Logger::Entry::SEVERITY severity)
{
	_cond_type = type;
	_cond_sever = severity;
}

void df::LogManager::HandleLogger(const LogEntry_Vec& vec, uint64_t frame_num_)
{
	for (auto entry : vec)
		AddLogEvent(entry, frame_num_);
}


bool df::LogManager::AddLogEvent(detail::Logger::Entry& entry, uint64_t frame_num_)
{
	if (_cond_type == CONDITION_TYPE::EQUALS && entry.severity == _cond_sever ||
		_cond_type == CONDITION_TYPE::NEQUALS && entry.severity != _cond_sever ||
		_cond_type == CONDITION_TYPE::ABOVE && static_cast<uint32_t>(entry.severity) > static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::ABOVE_EQ && static_cast<uint32_t>(entry.severity) >= static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::BELOW && static_cast<uint32_t>(entry.severity) < static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::BELOW_EQ && static_cast<uint32_t>(entry.severity) <= static_cast<uint32_t>(_cond_sever))
	{
		return true;
	}
	else
		return false;
}
