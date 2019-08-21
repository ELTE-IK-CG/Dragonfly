#pragma once
#include <type_traits>
#include <set>
#include "Shader.h"
#include <ImGui-addons/imgui_text_editor/TextEditor.h>

class SFileEditor : public SFile {
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
	SFileEditor() = delete;
	SFileEditor(const SFileEditor &) = delete;
	SFileEditor& operator=(const SFileEditor&) = delete;
	bool is_open = true, do_focus = true;
public:
	SFileEditor(SFileEditor &&o) = default;
	SFileEditor& operator=(SFileEditor&&) = default;
	//Create and Load shader file assosiated with path_
	SFileEditor(const std::string &path_) : SFile(path_), editor(nullptr) {}
	~SFileEditor() {}

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
	inline void ViewFile() const;
};

template<typename File_t>
class ShaderEditor : public Shader<File_t>{
	static_assert(std::is_base_of_v<SFile, File_t>,"File_t must be derived from SFile");
	using Base = Shader<File_t>;
private:
	void renderFileSelector();
	void onCompile();
	void hoverPath(const std::string& path);
	void renderShaderFiles(); // Need to specialize this

	struct {
		float height = 100.f;
		size_t selected_shader = 0;
		std::map<std::string, size_t> active_paths;
	} selector;
	using ErrorLine = SFileEditor::ErrorLine;
	struct {
		std::vector<ErrorLine> parsed_errors;
		TextEditor generated;
	}error_handling;
	struct {
		std::unique_ptr<File_t> file;
		size_t frames = 0;
		size_t idx = -1;
	} hover;
protected:
	struct DirEntry{
		std::vector<std::string> path;
		bool is_dir = false; // is_open = false;
	};
	std::vector<DirEntry> folders;
	std::string directory, name;
	ShaderEditor() = delete;
public:
	// You can also specify a directory view and a name so the shader retains its state via a config file
	ShaderEditor(GLenum type, const std::string &directory_ = "", const std::string &name = "");
	~ShaderEditor() = default;

	// Reload or Save the config file
	void Load();
	void Save();

	//Render Shader Editor
	void Render();

	//Optimizations need one update per frame
	void Update();

	//Gathers source code from added shaders and compiles (does not reload shaders)
	bool Compile();
};