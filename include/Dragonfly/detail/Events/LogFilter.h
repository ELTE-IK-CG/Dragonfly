#pragma once

#include "Logger.h"

namespace df {
	class LogFilter
	{
	private:
		bool _severity_filters[9];
		int _frame_from = -1;
		int _frame_to = -1;

	public:
		LogFilter() : _severity_filters{ false } {}

		[[nodiscard]] bool IsSubsetOf(LogFilter& f2);
		[[nodiscard]] bool Accept(const detail::Logger::Entry&, uint64_t) const;

		void SetSeverity(int s, bool v)
		{
			_severity_filters[s] = v;
		}

		void SetFrameLimits(const int from_, const int to_)
		{
			_frame_from = from_;
			_frame_to = to_;
		}
	};
}

