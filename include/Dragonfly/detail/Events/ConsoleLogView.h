#pragma once

#include "Dragonfly/detail/Events/LogHandlerBase.h"

namespace df {
	class ConsoleLogView : public LogHandlerBase
	{
	public:
		ConsoleLogView(LogManager* logManager_) {
			logManager_->Subscribe(this);
		}
		
		void LogArrived(df::LogManager::Instance*) override;
	};
}

