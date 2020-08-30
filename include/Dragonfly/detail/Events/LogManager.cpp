#include "LogManager.h"
#include <algorithm>

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
//

void df::LogManager::Sort(const LogSortCriteria criteria_)
{
	std::sort(_filteredAndOrdered.begin(), _filteredAndOrdered.end(), [this, criteria_](const std::pair<Hash_Type, std::size_t>& a, const std::pair<Hash_Type, std::size_t>& b)-> bool
		{
			auto aa = _data.find(a.first)->second;
			auto bb = _data.find(b.first)->second;

			if (criteria_ == LogSortCriteria::TIMESTAMP)
				return aa.entry.timestamp < bb.entry.timestamp;

			if (criteria_ == LogSortCriteria::SEVERITY)
				return static_cast<uint8_t>(aa.entry.severity) < static_cast<uint8_t>(bb.entry.severity);

			const auto frame_a = aa.instances[a.second].frameNumber;
			const auto frame_b = bb.instances[b.second].frameNumber;
			if (criteria_ == LogSortCriteria::FRAME_NUMBER)
				return frame_a < frame_b;
		
			if (criteria_ == LogSortCriteria::MESSAGE_STR)
				return aa.entry.message < bb.entry.message;
		
			if (criteria_ == LogSortCriteria::COUNT)
				return aa.instances.size() < bb.instances.size();
		}
	);
}


void df::LogManager::Filter(LogFilter& new_filter_)
{
	if (new_filter_.IsSubsetOf(_current_filter))
	{
		const auto last_element = std::remove_if(_filteredAndOrdered.begin(), _filteredAndOrdered.end(), [this, new_filter_](
			const std::pair<Hash_Type, std::size_t>& e)-> bool
			{
				auto ee = _data.find(e.first);
				//return !new_filter_.Accept(ee->second.entry.severity, ee->second.instances[e.second].frameNumber);
				return !new_filter_.Accept(ee->second.entry, ee->second.instances[e.second].frameNumber);
			}
		);
		_filteredAndOrdered.resize(std::distance(_filteredAndOrdered.begin(), last_element));
	}
	else
	{
		// Rebuild _filteredAndOrdered
		
		_filteredAndOrdered.clear();
		for (auto& e : _data)
		{
			for (auto& i : e.second.instances)
				_addLogInstance(e.second.entry, i.frameNumber);
		}
	}
	_current_filter = new_filter_;
}


bool df::LogManager::AddLogEvent(detail::Logger::Entry& entry_, const uint64_t frame_num_)
{
	if (!_current_filter.Accept(entry_, frame_num_))
		return false;

	_addLogInstance(entry_, frame_num_);

	return true;
}

void df::LogManager::_addLogInstance(detail::Logger::Entry& entry_, const uint64_t frame_num_)
{
	const auto hash = HashEntry(entry_);
	auto it = _data.find(hash);
	if (it == _data.end())
	{
		LogData logData(entry_);
		logData.instances.emplace_back(LogData::Instance(frame_num_, entry_.timestamp));
		_data.emplace(hash, logData);
		_filteredAndOrdered.emplace_back(std::pair<Hash_Type, std::size_t>(hash, logData.instances.size() - 1));
	}
	else
		it->second.instances.emplace_back(LogData::Instance(frame_num_, entry_.timestamp));
}
