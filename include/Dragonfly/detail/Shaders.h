#pragma once

namespace df
{

struct NoShader;	//use this class to indicate left-out shader stages

template<typename FragComp_t, typename Vert_t = NoShader, typename Geom_t = NoShader, typename TesC_t = NoShader, typename TesE_t = NoShader>
struct Shaders
{
	using Comp = NoShader;
	using Frag = FragComp_t;
	using Vert = Vert_t;
	using Geom = Geom_t;
	using TesC = TesC_t;
	using TesE = TesE_t;
};

template<typename Compute_t>
struct Shaders<Compute_t, NoShader, NoShader, NoShader, NoShader>
{
	using Comp = Compute_t;
	using Frag = NoShader;
	using Vert = NoShader;
	using Geom = NoShader;
	using TesC = NoShader;
	using TesE = NoShader;
};

template<typename File_t>
class Shader;
class SFile;

using ShaderVF = Shaders<Shader<SFile>, Shader<SFile>>;
using ShaderVGF = Shaders<Shader<SFile>, Shader<SFile>, Shader<SFile>>;
using ShaderVTF = Shaders<Shader<SFile>, Shader<SFile>, NoShader, Shader<SFile>, Shader<SFile>>;
using ShaderVGTF = Shaders<Shader<SFile>, Shader<SFile>, Shader<SFile>, Shader<SFile>, Shader<SFile>>;
using ShaderCompute = Shaders<Shader<SFile>>;

template<typename File_t>
class ShaderEditor;
class FileEditor;

using ShaderEditorVF = Shaders<ShaderEditor<FileEditor>, ShaderEditor<FileEditor>>;
using ShaderEditorVGF = Shaders<ShaderEditor<FileEditor>, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>>;
using ShaderEditorVTF = Shaders<ShaderEditor<FileEditor>, ShaderEditor<FileEditor>, NoShader, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>>;
using ShaderEditorVGTF = Shaders<ShaderEditor<FileEditor>, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>>;
using ShaderEditorCompute = Shaders<ShaderEditor<FileEditor>>;

} // namespace df
