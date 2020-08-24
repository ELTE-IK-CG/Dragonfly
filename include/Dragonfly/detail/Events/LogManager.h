#pragma once

#include <unordered_map>
#include <iostream>
#include <utility>
#include "LogFilterHelper.h"
#include "LogFilter.h"
#include "Logger.h"

namespace df {
	class LogManager {
	public:
		using Hash_Type = uint64_t; //hash from filePath + line + message

		struct LogData {
			struct Instance {
				uint64_t frameNumber;
				uint64_t timestamp;

				Instance(const uint64_t frameNumber_, const uint64_t timestamp_)
				{
					frameNumber = frameNumber_;
					timestamp = timestamp_;
				}
			};
			detail::Logger::Entry entry;
			std::vector<Instance> instances;

			explicit LogData(detail::Logger::Entry& entry_) : entry(entry_) {}
		};

		struct EntryData
		{
			detail::Logger::Entry entry;
			uint64_t frameNumber;
			uint64_t instanceCount;

			EntryData(detail::Logger::Entry entry_, const uint64_t frame_, const uint64_t instances_) : entry(std::move(entry_)), frameNumber(frame_), instanceCount(instances_) {}
		};

		enum class LogSortCriteria
		{
			TIMESTAMP,
			FRAME_NUMBER,
			SEVERITY,
			MESSAGE_STR
		};

	private:
		std::unordered_map<Hash_Type, LogData> _data;
		std::vector<std::pair<Hash_Type, std::size_t>> _filteredAndOrdered;
		LogFilter _current_filter;
		// LogSortCriteria _current_sort_criteria;
		// hash + index in instances

		static inline uint64_t HashEntry(detail::Logger::Entry&);

	public:
		void HandleLogger(const LogEntry_Vec&, uint64_t frame_num_);
		bool AddLogEvent(detail::Logger::Entry&, uint64_t frame_num_);

		void Sort(LogSortCriteria criteria_);
		void Filter(LogFilter& filter_);

		[[nodiscard]] size_t GetEntryCount() const { return _filteredAndOrdered.size(); }

		[[nodiscard]] EntryData Get(const size_t index_) const
		{
			const auto& ind = _filteredAndOrdered[index_];
			auto entry_data = _data.at(ind.first);
			const auto ret = entry_data.entry;
			const auto frame_number = entry_data.instances[ind.second].frameNumber;

			return EntryData(ret, frame_number, entry_data.instances.size());
		}
	};

}	//namespace df
