#include <filesystem>
#include <fstream>
#include <string>
#include "../../config.h"
#include "FileEditor.h"

using namespace df;

void FileEditor::CreateEditor() const{
	frames_unseen = 0;
	if (!editor) {
		editor = std::make_unique<TextEditor>();
		editor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
		editor->SetShowWhitespaces(false);
		auto palette = TextEditor::GetDarkPalette();
		palette[(int)TextEditor::PaletteIndex::Punctuation] = ImGui::ColorConvertFloat4ToU32({ 1,1,0.5,1 });
		editor->SetPalette(palette);
		editor->SetText(code);
		editor->SetErrorMarkers(error_markers);
	}
}

void FileEditor::SetErrorMarkers(const TextEditor::ErrorMarkers & errm) {
	//CreateEditor();
	error_markers = errm;
	if(editor) editor->SetErrorMarkers(error_markers);
}

void FileEditor::Render()
{
	std::string winname = "File Editor [" + path + ']';
	ImGui::PushID(winname.c_str());
	if (ImGui::CollapsingHeader(winname.c_str(), ImGuiTreeNodeFlags_None)) {
		CreateEditor();//only if needed!
		//ImVec2 region = ImGui::GetContentRegionMax();
		if (ImGui::BeginChild("Buttons", { 0, (error_msg.empty() ? 20.f : 40.f) + 15 }))
		{
			ImVec2 region = ImGui::GetContentRegionAvail();
			if (ImGui::Button("Save", { region.x*0.49f, 16.f + region.y*0.05f })) Save();
			ImGui::SameLine();
			if (ImGui::Button("Load", { region.x*0.49f, 16.f + region.y*0.05f })) { Load(); editor->SetText(code); }
			if (!error_msg.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, { 1,0.5f,0.5f,1 });
				ImGui::TextUnformatted(error_msg.c_str(), error_msg.c_str() + error_msg.length());
				ImGui::PopStyleColor();
			}

		}		ImGui::EndChild();
		editor->SetReadOnly(false); editor->SetHandleKeyboardInputs(true); editor->SetHandleMouseInputs(true);
		editor->Render(winname.c_str(), {0,450}, false);
		if (editor->IsTextChanged()) {
			code = editor->GetText();
			dirty = true;
		}
	}
	ImGui::PopID();
}

void FileEditor::ViewFile() const {
	CreateEditor();
	ImGui::BeginTooltip();
	editor->SetReadOnly(true); editor->SetHandleKeyboardInputs(false); editor->SetHandleMouseInputs(false);
	ImGui::TextColored({ 0.5,0.5,1.f,1 }, path.c_str());
	ImGui::Separator();
	editor->GetTotalLines();
	editor->SetImGuiChildIgnored(true);
	std::string winname = "File Editor [" + path + ']';
	editor->Render(winname.c_str(), { 0,0 }, false);
	editor->SetImGuiChildIgnored(false);
	ImGui::EndTooltip();
}
