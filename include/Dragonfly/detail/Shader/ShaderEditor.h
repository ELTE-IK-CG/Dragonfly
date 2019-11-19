#pragma once
#include "Shader.h"
#include "../File/FileEditor.h"
#include <ImGui-addons/imgui_text_editor/TextEditor.h>

namespace df
{

template<typename File_t>
class ShaderEditor : public Shader<File_t>{
	static_assert(std::is_base_of_v<SFile, File_t>,"File_t must be derived from SFile");
	using Base = Shader<File_t>;
private:
	void renderFileSelector();
	void renderErrorWindow();
	void onCompile();
	void hoverPath(const std::string& path);
	void renderShaderFiles(); // Need to specialize this

	struct {
		float height = 100.f;
		size_t selected_shader = 0;
		std::map<std::string, size_t> active_paths;
	} selector;
	using ErrorLine = FileEditor::ErrorLine;
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
	bool first_time_load = true;
public:
	// You can also specify a directory view and a name so the shader retains its state via a config file
	ShaderEditor(GLenum type, const std::string &directory_ = "", const std::string &name = "");
	~ShaderEditor() = default;

	// Reload or Save the config file
	void Load();
	void Save();

	//Render Shader Editor
	void Render(std::string program_name = "default");

	//Optimizations need one update per frame
	void Update();

	//Gathers source code from added shaders and compiles (does not reload shaders)
	bool Compile();
};

} //namespace df
