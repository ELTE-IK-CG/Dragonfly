#pragma once

#include <unordered_map>
#include "Logger.h"

namespace df {

	namespace detail // maybe to utility.h?
	{
		template <typename TFirst>
		void CombineHashes(TFirst first) {}

		template <typename TFirst, typename ... TRest>
		void CombineHashes(TFirst& first, TRest... rest)
		{
			first ^= (CombineHashes<TRest>(rest...) + 0x9e3779b1);
		}
	}

	class LogManager {
	public:
		enum class CONDITION_TYPE : uint8_t
		{
			EQUALS = 0,
			NEQUALS = 1,
			ABOVE = 2,
			ABOVE_EQ = 3,
			BELOW = 4,
			BELOW_EQ = 5,
		};

	private:
		CONDITION_TYPE _cond_type;
		detail::Logger::Entry::SEVERITY _cond_sever;

	public:
		using Hash_Type = uint64_t; //hash from filePath + line + message

		struct LogData {
			struct Instance {
				uint64_t frameNumber;
				uint64_t timestamp;
			};
			detail::Logger::Entry entry;
			std::vector<Instance> instances;
		};

		void SetFilterCondition(CONDITION_TYPE type, detail::Logger::Entry::SEVERITY severity);

	private:
		std::unordered_map<Hash_Type, LogData> data;
		std::vector<std::pair<Hash_Type, std::size_t>> filteredAndOrdered;

		inline uint64_t HashEntry(detail::Logger::Entry&) const;

	public:
		void HandleLogger(const LogEntry_Vec&, uint64_t frame_num_);
		bool AddLogEvent(detail::Logger::Entry&, uint64_t frame_num_);
	};

}	//namespace df
