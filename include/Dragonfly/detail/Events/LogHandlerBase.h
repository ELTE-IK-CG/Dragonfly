#pragma once

#include "Dragonfly/detail/Events/LogManager.h"

namespace df {
class LogHandlerBase
{
public:
	virtual void LogArrived(df::LogManager::Instance*);
};
}