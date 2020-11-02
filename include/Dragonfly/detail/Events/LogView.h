#ifndef LOG_TEST_LOGVIEW_H
#define LOG_TEST_LOGVIEW_H

#include "LogManager.h"
#include "LogFilter.h"
#include "LogHandlerBase.h"

namespace df {
	class LogView : public LogHandlerBase {
	public:
		enum class LogSortCriteria
		{
			TIMESTAMP,
			FRAME_NUMBER,
			COUNT,
			SEVERITY,
			MESSAGE_STR,
			NONE
		};

	private:
		LogManager* _logManager;
		LogFilter* _currentFilter;
		LogSortCriteria _currentCriteria;
		bool singleInstance;
		std::vector<LogManager::Instance*> _filteredAndOrdered;

	public:
		LogView(LogManager* logManager_) {
			_logManager = logManager_;
			_currentFilter = new LogFilter();
			logManager_->Subscribe(this);
		}

		LogManager::Instance* GetEntryAt(int index);
		void LogArrived(LogManager::Instance* instance) override;
		void SetFilter(LogFilter& filter);
		void SetSort(LogSortCriteria criteria);
		void SetSingleInstance(bool singleInstance_);

	private:
		void rebuildInternalList(bool optimizedFilter);
	};
}


#endif //LOG_TEST_LOGVIEW_H
