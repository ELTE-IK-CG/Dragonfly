#include "LogWindow.h"
#include <ImGui/imgui.h>
#include <string>
#include <iostream>

void df::LogWindow::Render()
{
	ImGui::SetNextWindowSize(ImVec2(600, 520), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Log Manager"))
	{
		if (ImGui::Button("Clear")) {}
		ImGui::SameLine();
		if (ImGui::Button("Filters"))
			ImGui::OpenPopup("Filtering");
		ImGui::Separator();

		if (ImGui::BeginPopup("Filtering"))
		{
			ImGui::Checkbox("Trace", &_level_trace);
			ImGui::SameLine();
			ImGui::Checkbox("Warning", &_level_warning);
			ImGui::Checkbox("Debug", &_level_debug);
			ImGui::SameLine();
			ImGui::Checkbox("Alarm", &_level_alarm);
			ImGui::Checkbox("Info ", &_level_info);
			ImGui::SameLine();
			ImGui::Checkbox("Error", &_level_error);
			ImGui::Checkbox("Hint ", &_level_hint);
			ImGui::SameLine();
			ImGui::Checkbox("Fatal", &_level_fatal);
			ImGui::Checkbox("Notice", &_level_notice);

			ImGui::EndPopup();
		}

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar))
		{
			const auto scroll_y = ImGui::GetScrollY();
			const auto viewport_height = ImGui::GetWindowHeight();
			const auto line_height = ImGui::GetTextLineHeightWithSpacing();

			const auto start_item = int(scroll_y / line_height);
			const auto end_item = int((scroll_y + viewport_height) / line_height);

			auto entries = Logger.GetEntries();

			for (int i = 0; i < entries.size(); i++)
			{
				ImGui::SetCursorPosY(i * line_height);
				if (i < start_item || i > end_item)
					continue;

				_renderLogEntry(entries[i]);
			}
			ImGui::SetCursorPosY(5 * line_height);
			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void df::LogWindow::_renderLogEntry(const detail::Logger::Entry entry_)
{
	ImVec4 color;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::TRACE)
		color = LOG_COLOR_ERROR;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::DEBUG)
		color = LOG_COLOR_DEBUG;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::INFO)
		color = LOG_COLOR_INFO;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::HINT)
		color = LOG_COLOR_HINT;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::NOTICE)
		color = LOG_COLOR_NOTICE;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::WARNING)
		color = LOG_COLOR_WARNING;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::ALARM)
		color = LOG_COLOR_ALARM;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::ERROR)
		color = LOG_COLOR_ERROR;
	if (entry_.severity == detail::Logger::Entry::SEVERITY::FATAL)
		color = LOG_COLOR_FATAL;

	auto expr_data = entry_.expression.data();

	if (*expr_data != 0)
		ImGui::TextColored(color, "[%llu] %s: %s", entry_.timestamp, entry_.expression.data(), entry_.message.c_str());
	else
		ImGui::TextColored(color, "[%llu] %s", entry_.timestamp, entry_.message.c_str());
}
