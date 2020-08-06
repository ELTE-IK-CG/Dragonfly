#pragma once

#include "Logger.h"
#include "LogManager.h"

#define LOG_COLOR_RGB(r, g, b) ImVec4((r), (g), (b), 1)
#define LOG_COLOR_TRACE LOG_COLOR_RGB(0.4, 0.4, 0.6)
#define LOG_COLOR_DEBUG LOG_COLOR_RGB(0.4, 0.4, 0.4)
#define LOG_COLOR_INFO LOG_COLOR_RGB(1, 1, 1)
#define LOG_COLOR_HINT LOG_COLOR_RGB(0.6, 1, 0.6)
#define LOG_COLOR_NOTICE LOG_COLOR_RGB(1, 1, 0.2)
#define LOG_COLOR_WARNING LOG_COLOR_RGB(1, 0.6, 0.2)
#define LOG_COLOR_ALARM LOG_COLOR_RGB(1, 0.6, 0.2) // ???
#define LOG_COLOR_ERROR LOG_COLOR_RGB(1, 0.3, 0.3)
#define LOG_COLOR_FATAL LOG_COLOR_RGB(1, 0, 0)

namespace df
{
	class LogWindow
	{
	private:
		bool _level_trace = false;
		bool _level_debug = false;
		bool _level_info = false;
		bool _level_hint = false;
		bool _level_notice = false;

		bool _level_warning = false;
		bool _level_alarm = false;
		bool _level_error = false;
		bool _level_fatal = false;

		// LogManager logManager;

	public:
		// explicit LogWindow(LogManager& logManager_) : logManager(logManager_) {}

		void Render();

	private:
		void _renderLogEntry(detail::Logger::Entry);
	};
}

