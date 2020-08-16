#include "LogManager.h"


uint64_t _Combine64bitHashes_3(uint64_t a_, uint64_t b_, uint64_t c_)
{
	return a_ ^ (b_ ^ (c_ + 0x9e3779b1) + 0x9e3779b1);
}

uint64_t df::LogManager::HashEntry(detail::Logger::Entry& entry_)
{
	const auto h1 = std::hash<std::string>{}(entry_.message);
	const auto h2 = std::hash<std::string_view>{}(entry_.location.filePath);
	const auto h3 = entry_.location.line;

	return _Combine64bitHashes_3(h1, h2, h3);
}

void df::LogManager::HandleLogger(const LogEntry_Vec& vec_, const uint64_t frame_num_)
{
	for (auto entry : vec_)
		AddLogEvent(entry, frame_num_);
}

// std::unordered_map<Hash_Type, LogData>			_data;
// std::vector<std::pair<Hash_Type, std::size_t>>	_filteredAndOrdered;

bool df::LogManager::AddLogEvent(detail::Logger::Entry& entry_, const uint64_t frame_num_)
{
	if (!filter.Accept(entry_.severity, frame_num_))
		return false;

	const auto hash = HashEntry(entry_);
	auto it = _data.find(hash);
	// LogData* logData;
	if (it == _data.end())
	{
		LogData logData(entry_);
		logData.instances.emplace_back(LogData::Instance(frame_num_, entry_.timestamp));
		_data.emplace(hash, logData);
		_filteredAndOrdered.emplace_back(std::pair<Hash_Type, std::size_t>(hash, logData.instances.size() - 1));
		std::cout << "Added item to _fAO " << GetEntryCount() << std::endl;
	}
	else
		it->second.instances.emplace_back(LogData::Instance(frame_num_, entry_.timestamp));

	return true;
}
