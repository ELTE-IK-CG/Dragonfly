#include "LogManager.h"


uint64_t _Combine64bitHashes_3(uint64_t a, uint64_t b, uint64_t c)
{
	return a ^ (b ^ (c + 0x9e3779b1) + 0x9e3779b1);
}

uint64_t df::LogManager::HashEntry(detail::Logger::Entry& entry) const
{
	const uint64_t h1 = std::hash<std::string>{}(entry.message);
	const uint64_t h2 = std::hash<std::string_view>{}(entry.location.filePath);
	const uint64_t h3 = entry.location.line;

	return _Combine64bitHashes_3(h1, h2, h3);
}

void df::LogManager::HandleLogger(const LogEntry_Vec& vec, uint64_t frame_num_)
{
	for (auto entry : vec)
		AddLogEvent(entry, frame_num_);
}


bool df::LogManager::AddLogEvent(detail::Logger::Entry& entry, uint64_t frame_num_)
{
	if (!filter.Accept(entry.severity, frame_num_))
		return false;

	return true;
}
