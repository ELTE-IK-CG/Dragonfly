#pragma once

#include <array>
#include <string>


#include "ImGui/imgui.h"
#include "LogView.h"

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

		std::string log_names[9] = {
			"TRACE",
			"DEBUG",
			"INFO",
			"HINT",
			"NOTICE",
			"WARNING",
			"ALARM",
			"ERROR",
			"FATAL"
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

		int _frm_start = -1;
		int _frm_end = -1;

		bool _reverse_sort = false;
		df::LogView* _log_view;
		df::LogView::LogSortCriteria _sort_criteria;

	public:
		LogWindow(LogView* log_view_) : _log_view(log_view_)
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
		void _renderLogEntry(const df::LogManager::Instance*);

		bool _canShowSeverity(df::LogManager::SEVERITY sev) const
		{
			return sev == df::LogManager::SEVERITY::TRACE && _level_trace ||
				sev == df::LogManager::SEVERITY::DEBUG && _level_debug ||
				sev == df::LogManager::SEVERITY::INFO && _level_info ||
				sev == df::LogManager::SEVERITY::HINT && _level_hint ||
				sev == df::LogManager::SEVERITY::NOTICE && _level_notice ||
				sev == df::LogManager::SEVERITY::WARNING && _level_warning ||
				sev == df::LogManager::SEVERITY::ALARM && _level_alarm ||
				sev == df::LogManager::SEVERITY::ERROR && _level_error ||
				sev == df::LogManager::SEVERITY::FATAL && _level_fatal;
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
			filter.SetFrameLimits(_frm_start, _frm_end);
			_log_view->SetFilter(filter);
		}

		void _updateFrameLimits()
		{
			if (strlen(_frm_start_buf))
				_frm_start = atoi(_frm_start_buf);
			if (strlen(_frm_end_buf))
				_frm_end = atoi(_frm_end_buf);

			_updateFilter();
		}

		void _setSort(df::LogView::LogSortCriteria criteria_)
		{
			_sort_criteria = criteria_;
			_log_view->SetSort(criteria_);
		}

		void _drawSortButton(df::LogView::LogSortCriteria criteria_, const std::string& text_, bool last_, ImVec2 size_ = ImVec2(0, 0));

		static void _drawArrow(bool up, ImVec2 pos);
	};
}

