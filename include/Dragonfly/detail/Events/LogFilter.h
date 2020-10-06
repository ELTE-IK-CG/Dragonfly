#ifndef LOG_TEST_LOGFILTER_H
#define LOG_TEST_LOGFILTER_H

#include <cstdint>
#include "LogManager.h"

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
		[[nodiscard]] bool Accept(const LogManager::Instance&) const;

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


#endif //LOG_TEST_LOGFILTER_H
