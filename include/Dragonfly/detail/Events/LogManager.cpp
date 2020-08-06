#include "LogManager.h"


uint64_t _Combine64bitHashes_3(uint64_t a_, uint64_t b_, uint64_t c_)
{
	return a_ ^ (b_ ^ (c_ + 0x9e3779b1) + 0x9e3779b1);
}

uint64_t df::LogManager::HashEntry(detail::Logger::Entry& entry_) const
{
	const uint64_t h1 = std::hash<std::string>{}(entry_.message);
	const uint64_t h2 = std::hash<std::string_view>{}(entry_.location.filePath);
	const uint64_t h3 = entry_.location.line;

	return _Combine64bitHashes_3(h1, h2, h3);
}

void df::LogManager::HandleLogger(const LogEntry_Vec& vec_, const uint64_t frame_num_)
{
	for (auto entry : vec_)
		AddLogEvent(entry, frame_num_);
}


bool df::LogManager::AddLogEvent(detail::Logger::Entry& entry_, const uint64_t frame_num_)
{
	if (!filter.Accept(entry_.severity, frame_num_))
		return false;

	auto hash = HashEntry(entry_);
	auto it = _data.find(hash);
	LogData* logData;
	if (it == _data.end())
		logData = new LogData(entry_);
	else
		logData = &it->second;
	logData->instances.emplace_back(LogData::Instance(frame_num_, entry_.timestamp));

	return true;
}
