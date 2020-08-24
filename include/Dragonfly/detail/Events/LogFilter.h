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
		[[nodiscard]] bool Accept(detail::Logger::Entry::SEVERITY, uint64_t) const;
		void SetSeverity(int s, bool v)
		{
			_severity_filters[s] = v;
		}
	};
}

