#include <filesystem>
#include <algorithm>
#include <fstream>
#include <string>
#include <regex>
#include "../../config.h"
#include "Shader.h"
#include "Shader.inl"
#include "ShaderEditor.h"

#include "../File/File.h"
#include "../File/FileEditor.h" // todo fix File template classes

using namespace df;

template<typename File_t>
ShaderEditor<File_t>::ShaderEditor(GLenum type, const std::string &directory_, const std::string &name_) : Base(type), directory(directory_), name(name_) {
	std::filesystem::path dirpath = directory_ == "" ? std::filesystem::current_path() : std::filesystem::path(directory_);
	for (const auto &dir : std::filesystem::recursive_directory_iterator(dirpath)) {
		if (dir.is_directory() || (dir.is_regular_file() && (dir.path().extension() == ".glsl" || dir.path().extension() == '.' + name))) {
			folders.emplace_back();
			folders.back().is_dir = dir.is_directory();
			for (auto p : dir.path())
				folders.back().path.push_back(p.generic_string());
		}
	}
	Load();
	error_handling.generated.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	error_handling.generated.SetShowWhitespaces(false);
	auto palette = TextEditor::GetDarkPalette();
	palette[(int)TextEditor::PaletteIndex::Punctuation] = ImGui::ColorConvertFloat4ToU32({ 1,1,0.5,1 });
	error_handling.generated.SetPalette(palette);
	error_handling.generated.SetReadOnly(true);
}

template<typename File_t>
void ShaderEditor<File_t>::Load(){

	if (!first_time_load) {
		this->shaders.clear();
		std::string path = directory + '/' + name + "_" + this->getTypeStr() + "_shader.config";
		if (std::ifstream in(path); in.is_open()) {
			while (std::getline(in, path)) {
				this->operator<<(path);
			}
			in.close();
		}
	}
	first_time_load = false;

}

template<typename File_t>
void ShaderEditor<File_t>::Save() {
	std::string path = directory + '/' + name + "_" + this->getTypeStr() + "_shader.config";
	std::vector<std::string> path_names;

	if (std::ofstream out(path, std::ofstream::trunc); out.is_open()){
		for (const auto& sh : this->shaders) {
			if(std::find(path_names.begin(), path_names.end(), sh.GetPath()) == path_names.end() )
				path_names.push_back(sh.GetPath());
		}

		for (std::string sh : path_names) {
			out << sh << std::endl;
		}
		out.close();
	}
	else ASSERT(false,"Saving failed");
}

template<typename File_t> void ShaderEditor<File_t>::Update() {
	Base::Update();
	if (hover.file) hover.file->Update();
}

template<typename File_t>
bool ShaderEditor<File_t>::Compile()
{
	bool b = Shader<File_t>::Compile(); onCompile(); return b;
}

template<typename File_t>
void ShaderEditor<File_t>::Render(std::string program_name){
	
	std::string winname = "Shader Editor [" + directory  + (name.empty() ? "] [" : "] [" + name + "] [") + this->getTypeStr() + "] ";
	ImGui::PushID(ImGui::GetID(winname.c_str()));
	
	if (ImGui::Begin(program_name.c_str()))
	{
			if (ImGui::BeginChild(this->getTypeStr().c_str(), ImGui::GetContentRegionAvail())) {
			ImVec2 region = ImGui::GetContentRegionAvail();
			std::string path = directory + '/' + name + "_" + this->getTypeStr() + "_shader.config";

			int tab = -1;
			ImGui::PushID(ImGui::GetID(winname.c_str()));
			if (ImGui::BeginTabBar("ShaderTabBar", 0)) {
				if (ImGui::BeginTabItem("Shader Build Setup")) {
					tab = 0;
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Generated Source Code")) {
					tab = 1;
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Code Editor")) {
					tab = 2;
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::PopID();

			switch (tab) {
			case 0:
				this->renderFileSelector();		//Shader File selector
				this->renderErrorWindow();
				break;
			case 1:
				error_handling.generated.Render("Gencode");
				break;
			case 2:
				this->renderShaderFiles();		//Templates are specialized
			default:break;
			}

		}ImGui::EndChild();
	}	ImGui::End();
	ImGui::PopID();
}

template<typename File_t>
void hoverFileImp(const File_t &file) {
	static_assert(true, "Wrong File_t type. Use the correct template specialization.");
}

template<typename File_t>
void ShaderEditor<File_t>::renderFileSelector()
{
	ImVec2 region = ImGui::GetContentRegionAvail();
	std::string path = directory + '/' + name + "_" + this->getTypeStr() + "_shader.config";
	ImGui::PushID(path.c_str());
	if (ImGui::CollapsingHeader("Shader File Selector", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool focus = false;
		float height = selector.height;
		if (ImGui::BeginChild("Folders", { region.x*0.54f-1.f, height+10.f }, true, ImGuiWindowFlags_NoNav))
		{
			focus |= ImGui::IsWindowFocused();
			ImVec2 region = ImGui::GetContentRegionAvail();
			size_t recursion_skip_depth = -1;
			for (size_t i = 0; i < folders.size(); ++i) {
				auto& e = folders[i]; if (e.path.size() > recursion_skip_depth) continue; //parent folder treenode is closed

				ImGui::Indent(region.x*0.15f*(e.path.size()-0.9f) + 1.f); ImGui::PushID((int)i);
				if (e.is_dir) {	//folder
					bool b_open_dir = ImGui::TreeNodeEx(e.path.back().c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen);
					recursion_skip_depth = (b_open_dir ? -1 : e.path.size());
				}else{			//file
					std::string path; for (auto s : e.path)	path += s + '/';
					path.pop_back(); path.shrink_to_fit();
					if (selector.active_paths.count(path) == 0) {	//file does not exist
						float pos =	ImGui::GetCursorPosX();
						ImGui::Selectable("##F", false, 0, { 0,0 });
						bool bhover = ImGui::IsItemHovered();
						bool bclick = bhover && (ImGui::IsMouseClicked(1) || ImGui::IsMouseDoubleClicked(0)); //on right and doubble click
						ImGui::PushStyleColor(ImGuiCol_Text, { 0.5,0.5,1,1 });
						ImGui::SameLine(pos); ImGui::Bullet(); ImGui::TextUnformatted(e.path.back().c_str(),e.path.back().c_str() +e.path.back().length());
						ImGui::PopStyleColor();
						if (bclick) { this->operator<<(path); selector.active_paths.emplace(path, i); }
						if (bhover) {
							hoverPath(path);
							hover.idx = i;
						}
						else if(hover.idx == i) hover.frames = 0;
					}
				}
				ImGui::Unindent(region.x*0.15f*(e.path.size()-0.9f) + 1.f); ImGui::PopID();
			}
			selector.height = ImGui::GetCursorPosY();
		}	ImGui::EndChild(); ImGui::SameLine();

		if (ImGui::BeginChild("Compile List", { region.x*0.43f-1.f, height+10.f }, true, ImGuiWindowFlags_NoNav))
		{
			focus |= ImGui::IsWindowFocused();
			ImVec2 region = ImGui::GetContentRegionAvail();
			bool moved = false;
			for (size_t i=0; i < this->shaders.size(); ++i) {
				ImGui::PushID((int)i);
				const auto& sh = this->shaders[i];

				float pos =	ImGui::GetCursorPosX();
				bool select = ImGui::Selectable("##S", selector.selected_shader == i, 0, { region.x - 70.f,0 });
				bool bhover = ImGui::IsItemHovered();
				bool bclick = bhover && (ImGui::IsMouseClicked(1) || ImGui::IsMouseDoubleClicked(0)); //on right and doubble click
				ImGui::SameLine(pos); ImGui::TextColored({ 0.5,0.5,1,1 }, "%s/%s", sh.GetFolder().c_str(), sh.GetFilename().c_str());
				if (select) selector.selected_shader = i;
				if (bool down = false;  i + 1 < this->shaders.size()) {
					ImGui::SameLine(region.x - 30.f);
					down |= (selector.selected_shader == i) &&focus&& ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow),false);
					down |= ImGui::ArrowButton("D", ImGuiDir_Down);
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move the %s later in generated code.", sh.GetPath().c_str());
					if (down && !moved) {
						std::swap(this->shaders[i], this->shaders[i + 1]);
						++selector.selected_shader;
						moved = true;
					}
				}
				if (bool up = false; i > 0) {
					ImGui::SameLine(region.x - 10.f);
					up |= (selector.selected_shader == i) && focus && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow),false);
					up |= ImGui::ArrowButton("U", ImGuiDir_Up);
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move the %s later in generated code.", sh.GetPath().c_str());
					if (up) {
						std::swap(this->shaders[i], this->shaders[i - 1]);
						--selector.selected_shader;
					}
				if (bhover)	hoverFileImp(sh);
				if (bclick) { this->EraseShader(i); selector.active_paths.erase(sh.GetPath()); }
				}
				ImGui::PopID();
			}
			selector.height = std::max(ImGui::GetCursorPosY(),selector.height);
		}	ImGui::EndChild();
	}
	float wid = 0.1945f;
	// Buttons
	if (ImGui::Button("Load config", { region.x * wid, 12.f + region.y * 0.05f })) Load();
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Load shader file list from %s.", path.c_str());
	if (!this->shaders.empty()) {
		ImGui::SameLine();
		if (ImGui::Button("Save config", { region.x * wid, 12.f + region.y * 0.05f }))
			Save();
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save shader file list to %s.", path.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Clear", { region.x * wid, 12.f + region.y * 0.05f })) {
			this->shaders.clear();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Clear the current shader setup.");
		ImGui::SameLine();
		if (ImGui::Button("Pop Shader", { region.x * wid, 12.f + region.y * 0.05f })) {
			selector.active_paths.erase(this->shaders.back().GetPath());
			this->PopShader();
		}
		else if (ImGui::IsItemHovered()) ImGui::SetTooltip("Remove shader from compile list: %s.", this->shaders.back().GetPath());
		ImGui::SameLine();
		if (ImGui::Button("Compile", { region.x * wid, 12.f + region.y * 0.05f })) {
			Compile();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Compile %i shader files.", this->shaders.size());
	}
	ImGui::PopID();
}

template<typename File_t>
void ShaderEditor<File_t>::renderErrorWindow() {
	ImVec2 region = ImGui::GetContentRegionAvail();
	ImGui::PushID(ImGui::GetID(this->getTypeStr().c_str()));
	if (ImGui::CollapsingHeader("Error List", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginChild("Error Window", { 0,0 }, true, ImGuiWindowFlags_HorizontalScrollbar))
		{
			for (const auto& e : error_handling.parsed_errors) {
				float pos = ImGui::GetCursorPosX();
				ImGui::Selectable("##Error", false);
				ImGui::SameLine(pos);
				switch (e.type) {
				case ErrorLine::Type::ERROR: ImGui::PushStyleColor(ImGuiCol_Text, { 1,0.4f,0.4f,1.f }); ImGui::TextUnformatted("ERROR"); break;
				case ErrorLine::Type::WARNING: ImGui::PushStyleColor(ImGuiCol_Text, { 1,1,0,0.6f }); ImGui::TextUnformatted("WARNING"); break;
				case ErrorLine::Type::UNKNOWN: ImGui::PushStyleColor(ImGuiCol_Text, { 1,1,1,0.6f }); ImGui::TextUnformatted("UNKNOWN"); break;
				default:ImGui::PushStyleColor(ImGuiCol_Text, { 1,1,1,1 });	break;
				}
				ImGui::SameLine(pos + 45.f);		ImGui::Text("line %i", e.line);
				ImGui::SameLine(pos + 115.f);		ImGui::TextUnformatted(e.path.c_str(), e.path.c_str() + e.path.length());
				ImGui::SameLine(pos + 330.f);		ImGui::TextUnformatted(e.message.c_str(), e.message.c_str() + e.message.length());
				ImGui::PopStyleColor();
			}
			
		}	ImGui::EndChild();
	} 
	ImGui::PopID();
}

template <typename File_t>
void setMarkersImpl(File_t& sfile, const TextEditor::ErrorMarkers &markers) {}//do nothing
template<> void setMarkersImpl<FileEditor>(FileEditor& sfile, const TextEditor::ErrorMarkers &markers) {
	sfile.SetErrorMarkers(markers);
}


template<typename File_t>
void ShaderEditor<File_t>::onCompile() {
	//set text
	error_handling.generated.SetText("");
	error_handling.generated.SetReadOnly(false);
	std::vector<int> line_numbers(this->source_strs.size());
	for (int i = 0; i < this->source_strs.size(); ++i) {
		error_handling.generated.InsertText(this->source_strs[i]);
		line_numbers[i] = error_handling.generated.GetTotalLines();
	}
	error_handling.generated.SetReadOnly(true);
	//helper functions
	auto parseError = [](const std::string &line) -> ErrorLine {
		static std::regex r_i("((?:ERROR)|(?:WARNING)):\\s*(\\d+):(\\d+):\\s*(.*?)\\s*", std::regex::flag_type::optimize);						//intel compiler
		static std::regex r_n("(.*?)\\((\\d+)\\)\\s*:\\s*((?:error)|(?:warning))\\s*C(\\d{4}):\\s*(.*?)\\s*",std::regex::flag_type::optimize);	//nvidia compiler
		//static std::regex r_a("((?:ERROR)|(?:WARNING)):\\s*(\\d+):(\\d+):\\s*(.*?)\\s*",std::regex::flag_type::optimize);						//amd compiler
		std::smatch m; ErrorLine ret;
		if (std::regex_match(line, m, r_i))	{
			if (m[1] == "ERROR")			ret.type = ErrorLine::Type::ERROR;
			else if (m[1] == "WARNING")		ret.type = ErrorLine::Type::WARNING;
			ret.col = std::stoi(m[2]);		ret.line = std::stoi(m[3]);
			ret.message = m[4];
		}
		else if (std::regex_match(line, m, r_n)) {
			ret.path = m[1];				ret.line = std::stoi(m[2]);
			if (m[3] == "error")			ret.type = ErrorLine::Type::ERROR;
			else if (m[3] == "warning")		ret.type = ErrorLine::Type::WARNING;
			ret.e_code = std::stoi(m[4]);	ret.message = m[5];
		}
		else ret.message = line;
		return ret;
	};
	TextEditor::ErrorMarkers gen_markers;
	std::vector<TextEditor::ErrorMarkers> file_markers;
	if constexpr (std::is_same_v<FileEditor, File_t>) file_markers.resize(this->shaders.size());

	auto addError = [&](const std::string& errline) -> void {
		ErrorLine err = parseError(errline);
		if (!(line_numbers.front() <= err.line && err.line <= line_numbers.back())) {
			ASSERT(false, "Parsing the error probably failed.");
			error_handling.parsed_errors.emplace_back(err);
			return;
		}
		gen_markers.emplace(err.line, err.message);

		//find out which file has that error
		auto it_src_line_nbr = std::upper_bound(line_numbers.cbegin(), line_numbers.cend(), err.line) - 1;
		int total_lines_to_src = *it_src_line_nbr;
		ASSERT(0 <= total_lines_to_src && total_lines_to_src <= err.line, "Error finding exact line of the shader error message in file.");
		ASSERT(it_src_line_nbr + 1 == line_numbers.cend() || err.line <= *(it_src_line_nbr + 1), "Error finding exact line of the shader error message in file.");
		size_t which_source = std::distance(line_numbers.cbegin(), it_src_line_nbr);
		ASSERT(0 <= which_source && which_source < this->source_strs.size(), "Error finding exact line of the shader error message in file.");
		if (which_source == 0)
		{	// problem with version
			err.path = "WRONG GENERATED VERSION";
			err.message = "Check the first line of all shader files under compilation externally to this application. (" + err.message + ')';
		}
		else
		{
			size_t file_id = (which_source - 1) / 2;	// FOUND YOU! (I hope)
			ASSERT(0 <= file_id && file_id < this->shaders.size(), "Invalid file ID");

			err.path = this->shaders[file_id].GetFolder() + '/' + this->shaders[file_id].GetFilename() + this->shaders[file_id].GetExtension();

			//if(err.type == ErrorLine::Type::ERROR)	gen_markers.emplace(err.line, err.message);

			err.line = which_source % 2 == 1 ? //line number in file
				err.line - total_lines_to_src + 1:
				line_numbers[which_source]-1; //last line number of that previous file.

			if constexpr (std::is_same_v<FileEditor, File_t>) if (err.type == ErrorLine::Type::ERROR) file_markers[file_id].emplace(err.line, err.message);
		}
		error_handling.parsed_errors.emplace_back(err);
	};

	//error setup
	error_handling.parsed_errors.clear();
	if (!this->GetErrors().empty()) {
		size_t start = 0, end;
		while (true) {
			std::string this_line;
			if ((end = this->GetErrors().find("\n", start)) == std::string::npos) {
				if (!(this_line = this->GetErrors().substr(start)).empty())
					addError(this_line);
				break;
			}
			this_line = this->GetErrors().substr(start, end - start);
			addError(this_line);
			start = end + 1;
		}
	}
	error_handling.generated.SetErrorMarkers(gen_markers);
	if constexpr (std::is_same_v<FileEditor, File_t>) for (int i = 0; i < this->shaders.size(); ++i) setMarkersImpl(this->shaders[i], file_markers[i]);
}


// ========================= File_t specializations ==============================
// todo cleanup this mess

template<> void hoverFileImp(const SFile & file){
	ImGui::BeginTooltip();	ImGui::TextColored({ 0.5,0.5,1,1 }, file.GetPath().c_str());
	ImGui::Separator();
	ImGui::TextUnformatted(file.GetCode().c_str(), file.GetCode().c_str() + file.GetCode().size());
	ImGui::EndTooltip();
}
template<> void hoverFileImp(const FileEditor & file){	file.ViewFile();}

template<typename File_t>
void ShaderEditor<File_t>::hoverPath(const std::string & path) {
	if (++hover.frames < 12) {
		ImGui::BeginTooltip();	ImGui::TextColored({ 0.5,0.5,1,1 }, path.c_str());	ImGui::EndTooltip();
	} else if (hover.frames == 12 || path != hover.file->GetPath())
		hover.file = std::make_unique<FileEditor>(path);

	if (hover.frames >= 12) hoverFileImp(*hover.file);
}

template<> void ShaderEditor<SFile>::renderShaderFiles(){
	if (this->shaders.empty()) return;
	if (ImGui::BeginChild("ReadOnlyText"))	{
		const auto & sh= this->shaders[selector.selected_shader];
		ImGui::TextColored({ 0.5,0.5,1,1 }, sh.GetPath().c_str());
		ImGui::Separator();
		const std::string &code = this->shaders[selector.selected_shader].GetCode();
		ImGui::TextUnformatted(code.c_str(),code.c_str() + code.length());
	}	ImGui::EndChild();
}


//dont dock next to each other, but rather below
template<> void ShaderEditor<FileEditor>::renderShaderFiles() {
	for (auto& sh : shaders) {
			sh.Render();
		}
}



template class Shader<FileEditor>;
template class ShaderEditor<SFile>;
template class ShaderEditor<FileEditor>;