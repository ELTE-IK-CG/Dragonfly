#pragma once

#include "Logger.h"
#include "LogManager.h"
#include <array>

#include "ImGui/imgui.h"

// TODO constexpr array
#define LOG_COLOR_RGB(r, g, b) ImVec4((r), (g), (b), 1)

namespace df
{
	class LogWindow
	{
	private:
		ImVec4 log_colors[9] = {
			LOG_COLOR_RGB(0.4, 0.4, 0.6),	// TRACE	
			LOG_COLOR_RGB(0.4, 0.4, 0.4),	// DEBUG	
			LOG_COLOR_RGB(1, 1, 1),			// INFO	
			LOG_COLOR_RGB(0.6, 1, 0.6),		// HINT	
			LOG_COLOR_RGB(1, 1, 0.2),		// NOTICE	
			LOG_COLOR_RGB(1, 0.6, 0.2),		// WARNING 
			LOG_COLOR_RGB(1, 0.6, 0.2),		// ALARM	
			LOG_COLOR_RGB(1, 0.3, 0.3),		// ERROR	
			LOG_COLOR_RGB(1, 0, 0)			// FATAL	
		};

		bool _level_trace = true;
		bool _level_debug = true;
		bool _level_info = true;
		bool _level_hint = true;
		bool _level_notice = true;

		bool _level_warning = true;
		bool _level_alarm = true;
		bool _level_error = true;
		bool _level_fatal = true;

		char* _frm_start_buf;
		char* _frm_end_buf;

		LogManager* logManager;

	public:
		LogWindow(LogManager* logManager_) : logManager(logManager_)
		{
			_frm_start_buf = new char[64]{ 0 };
			_frm_end_buf = new char[64]{ 0 };
			_updateFilter();
		}

		/*LogWindow()
		{
			_frm_start_buf = new char[64]{0};
			_frm_end_buf = new char[64]{0};
		}*/

		void Render();

	private:
		void _renderLogEntry(const LogManager::EntryData&);

		bool _canShowSeverity(detail::Logger::Entry::SEVERITY sev) const
		{
			return sev == detail::Logger::Entry::SEVERITY::TRACE && _level_trace ||
				sev == detail::Logger::Entry::SEVERITY::DEBUG && _level_debug ||
				sev == detail::Logger::Entry::SEVERITY::INFO && _level_info ||
				sev == detail::Logger::Entry::SEVERITY::HINT && _level_hint ||
				sev == detail::Logger::Entry::SEVERITY::NOTICE && _level_notice ||
				sev == detail::Logger::Entry::SEVERITY::WARNING && _level_warning ||
				sev == detail::Logger::Entry::SEVERITY::ALARM && _level_alarm ||
				sev == detail::Logger::Entry::SEVERITY::ERROR && _level_error ||
				sev == detail::Logger::Entry::SEVERITY::FATAL && _level_fatal;
		}

		void _updateFilter()
		{
			LogFilter filter;
			filter.SetSeverity(0, _level_trace);
			filter.SetSeverity(1, _level_debug);
			filter.SetSeverity(2, _level_info);
			filter.SetSeverity(3, _level_hint);
			filter.SetSeverity(4, _level_notice);
			filter.SetSeverity(5, _level_warning);
			filter.SetSeverity(6, _level_alarm);
			filter.SetSeverity(7, _level_error);
			filter.SetSeverity(8, _level_fatal);
			logManager->Filter(filter);
		}
	};
}

