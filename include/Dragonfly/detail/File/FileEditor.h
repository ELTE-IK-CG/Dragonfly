#pragma once
#include "File.h"
#include <ImGui-addons/imgui_text_editor/TextEditor.h>

namespace df
{

class FileEditor : public SFile {
public:
	struct ErrorLine {
		enum class Type { UNKNOWN = 0, ERROR = 1, WARNING = 2 };
		Type type = Type::UNKNOWN;
		int line=-1, col = -1, e_code = 0;
		std::string path, message;
	};
private:
	mutable std::unique_ptr<TextEditor> editor = nullptr;	// should only exist while window is open... needs to prompt when closing...
	TextEditor::ErrorMarkers error_markers;	// Because text editor can do that too
	mutable int frames_unseen = 0;			// deletes TextEditor if not visible for long
	void CreateEditor() const;				// only if needed!
	inline void DeleteEditor() const {		// only not needed!
		if (++frames_unseen >= 120 && editor != nullptr)	editor.release();
	}
	FileEditor() = delete;
	FileEditor(const FileEditor &) = delete;
	FileEditor& operator=(const FileEditor&) = delete;
	bool is_open = true, do_focus = true;
	
public:
	FileEditor(FileEditor &&o) = default;
	FileEditor& operator=(FileEditor&&) = default;
	//Create and Load shader file assosiated with path_
	FileEditor(const std::string &path_) : SFile(path_), editor(nullptr) {}
	~FileEditor() {}

	// Set Error Markers: TODO: make it pretty(er)
	void SetErrorMarkers(const TextEditor::ErrorMarkers &errm);

	// Renders a window for editing the shader file.
	void Render();

	// Open (also focus) or Close window
	void Open() { is_open = do_focus = true; }
	void Close() { is_open = do_focus = false; }

	// Call it every frame once
	inline void Update() const	{ DeleteEditor(); } //will only delete when not visible for some time

	// Renders a tooltip window. Best used with ImGui::IsItemHoweverd()
	void ViewFile() const;
};

} //namespace df
