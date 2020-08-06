#pragma once

#include <unordered_map>
#include "LogFilterHelper.h"
#include "Logger.h"

namespace df {
	class LogManager {
	public:
		LogFilter filter;
		
		using Hash_Type = uint64_t; //hash from filePath + line + message

		struct LogData {
			struct Instance {
				uint64_t frameNumber;
				uint64_t timestamp;
			};
			detail::Logger::Entry entry;
			std::vector<Instance> instances;
		};

	private:
		std::unordered_map<Hash_Type, LogData> data;
		std::vector<std::pair<Hash_Type, std::size_t>> filteredAndOrdered; // hash + index in instances

		inline uint64_t HashEntry(detail::Logger::Entry&) const;

	public:
		void HandleLogger(const LogEntry_Vec&, uint64_t frame_num_);
		bool AddLogEvent(detail::Logger::Entry&, uint64_t frame_num_);
	};

}	//namespace df
