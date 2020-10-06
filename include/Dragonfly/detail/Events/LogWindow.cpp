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
			// Logger.ClearEntries();
		}
		ImGui::SameLine();
		if (ImGui::Button("Filters"))
			ImGui::OpenPopup("Filtering");
		ImGui::Separator();

		if (ImGui::BeginPopup("Filtering"))
		{
			if (ImGui::Checkbox("Trace", &_level_trace))
				_updateFilter();
			ImGui::SameLine();
			if (ImGui::Checkbox("Warning", &_level_warning))
				_updateFilter();
			if (ImGui::Checkbox("Debug", &_level_debug))
				_updateFilter();
			ImGui::SameLine();
			if (ImGui::Checkbox("Alarm", &_level_alarm))
				_updateFilter();
			if (ImGui::Checkbox("Info ", &_level_info))
				_updateFilter();
			ImGui::SameLine();
			if (ImGui::Checkbox("Error", &_level_error))
				_updateFilter();
			if (ImGui::Checkbox("Hint ", &_level_hint))
				_updateFilter();
			ImGui::SameLine();
			if (ImGui::Checkbox("Fatal", &_level_fatal))
				_updateFilter();
			if (ImGui::Checkbox("Notice", &_level_notice))
				_updateFilter();


			if (ImGui::InputText("Frame Start ", _frm_start_buf, 16, ImGuiInputTextFlags_CharsDecimal))
				_updateFrameLimits();
			if (ImGui::InputText("Frame End ", _frm_end_buf, 16, ImGuiInputTextFlags_CharsDecimal))
				_updateFrameLimits();

			ImGui::EndPopup();
		}


		_drawSortButton(df::LogView::LogSortCriteria::TIMESTAMP, "Timestamp ", false, ImVec2(104, 0));
		_drawSortButton(df::LogView::LogSortCriteria::FRAME_NUMBER, "Frame ", false);
		_drawSortButton(df::LogView::LogSortCriteria::COUNT, "Count ", false);
		_drawSortButton(df::LogView::LogSortCriteria::SEVERITY, "Severity ", false);
		_drawSortButton(df::LogView::LogSortCriteria::MESSAGE_STR, "Message ", true, ImVec2(ImGui::GetWindowWidth() - ImGui::GetCursorPosX(), 0));

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

			for (int i = 0; ; i++)
			{
				auto instance = _log_view->GetEntryAt(i);
				if (instance == nullptr)
					break;

				if (!_canShowSeverity(instance->entry->severity))
					continue;
				count++;
				ImGui::SetCursorPosY(index * line_height);
				if (i < start_item || i > end_item)
					continue;
				index++;

				_renderLogEntry(instance);
			}
			ImGui::SetCursorPosY(5 * line_height);
			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void df::LogWindow::_renderLogEntry(const df::LogManager::Instance* instance_)
{
	const auto color = log_colors[static_cast<uint32_t>(instance_->entry->severity)];
	const auto expr_data = instance_->entry->expression.data();

	//ImGui::TextColored(color, "[%llu]"",  entry_data_.entry.timestamp); ImGui::SameLine(50);
	//ImGui::TextColored(color, "[%llu]"",  entry_data_.frameNumber); ImGui::SameLine(50);


	/*if (*expr_data != 0)
		ImGui::TextColored(color, "[%llu] [%llu] (%llu) %s: %s", entry_data_.entry.timestamp, entry_data_.frameNumber, entry_data_.instanceCount, entry_data_.entry.expression.data(), entry_data_.entry.message.c_str());
	else
		ImGui::TextColored(color, "[%llu] [%llu] (%llu) %s", entry_data_.entry.timestamp, entry_data_.frameNumber, entry_data_.instanceCount, entry_data_.entry.message.c_str());*/

	ImGui::TextColored(color, "[%llu]", instance_->timestamp);
	ImGui::SameLine();
	ImGui::TextColored(color, "[%llu]", instance_->frame_number);
	ImGui::SameLine(178);
	ImGui::TextColored(color, "[%llu]", instance_->entry->instances.size());
	ImGui::SameLine(242);
	ImGui::TextColored(color, "[%s]", log_names[static_cast<int>(instance_->entry->severity)].c_str());
	ImGui::SameLine(330);
	ImGui::TextColored(color, "%s", instance_->entry->message.c_str());
}

void df::LogWindow::_drawSortButton(LogView::LogSortCriteria criteria_, const std::string& text_, bool last_, ImVec2 size_)
{
	const auto active = criteria_ == _sort_criteria;
	if (active)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2313, 0.4549, 0.7098, 1));
	const auto postfix = active && _reverse_sort ? std::string("V") : std::string(" ");
	if (ImGui::Button((text_ + postfix).c_str(), size_))
		_setSort(criteria_);
	if (!last_)
		ImGui::SameLine();
	if (active)
		ImGui::PopStyleColor();
}

void df::LogWindow::_drawArrow(bool up, ImVec2 pos)
{
	auto draw_list = ImGui::GetWindowDrawList();
	const ImVec2 half_sz(10, 10);
	const ImVec4 col(1, 0, 0, 1);
	// draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col);
}
