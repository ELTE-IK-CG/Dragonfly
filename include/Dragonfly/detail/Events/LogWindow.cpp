#include "LogWindow.h"
#include <ImGui/imgui.h>
#include <string>
#include <iostream>

void df::LogWindow::Render()
{
	ImGui::SetNextWindowSize(ImVec2(600, 520), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Log Manager"))
	{
		if (ImGui::Button("Clear"))
		{
			Logger.ClearEntries();
		}
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

			ImGui::InputText("Frame Start: ", _frm_start_buf, 16, ImGuiInputTextFlags_CharsDecimal);
			ImGui::InputText("Frame End: ", _frm_end_buf, 16, ImGuiInputTextFlags_CharsDecimal);

			ImGui::EndPopup();
		}

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar))
		{
			const auto scroll_y = ImGui::GetScrollY();
			const auto viewport_height = ImGui::GetWindowHeight();
			const auto line_height = ImGui::GetTextLineHeightWithSpacing();

			const auto start_item = int(scroll_y / line_height);
			const auto end_item = int((scroll_y + viewport_height) / line_height);

			//auto entries = Logger.GetEntries();

			int count = 0;
			int index = 0;

			//std::cout << logManager.GetEntryCount() << std::endl;
			for (int i = 0; i < logManager->GetEntryCount(); i++)
			{
				auto entry = logManager->Get(i);

				if (!_canShowSeverity(entry.entry.severity))
					continue;
				count++;
				ImGui::SetCursorPosY(index * line_height);
				if (i < start_item || i > end_item)
					continue;
				index++;

				_renderLogEntry(entry);
			}
			ImGui::SetCursorPosY(5 * line_height);
			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void df::LogWindow::_renderLogEntry(const LogManager::EntryData& entry_data_)
{
	const auto color = log_colors[static_cast<uint32_t>(entry_data_.entry.severity)];
	const auto expr_data = entry_data_.entry.expression.data();

	if (*expr_data != 0)
		ImGui::TextColored(color, "[%llu] [%llu] (%llu) %s: %s", entry_data_.entry.timestamp, entry_data_.frameNumber, entry_data_.instanceCount, entry_data_.entry.expression.data(), entry_data_.entry.message.c_str());
	else
		ImGui::TextColored(color, "[%llu] [%llu] (%llu) %s", entry_data_.entry.timestamp, entry_data_.frameNumber, entry_data_.instanceCount, entry_data_.entry.message.c_str());
}
