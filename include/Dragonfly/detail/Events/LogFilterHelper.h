#pragma once

#include <cstdint>
#include "Logger.h"

namespace df
{
	class LogFilterOld // TODO Delete
	{
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
		uint64_t _frame_from;
		uint64_t _frame_to;

		bool AcceptSeverity(detail::Logger::Entry::SEVERITY) const;
		bool AcceptFrameNumber(uint64_t) const;

	public:
		void SetFilterCondition(CONDITION_TYPE type, detail::Logger::Entry::SEVERITY severity);
		void SetFrameInterval(uint64_t from, uint64_t to);

		bool Accept(detail::Logger::Entry::SEVERITY, uint64_t) const;
	};
}
