#include "Spirver.h"
#include <glslang/SPIRV/SpvTools.h>
#include <glslang/SPIRV/GlslangToSpv.h>


const char* Spirver::stageNames[] = { "Vertex", "Fragment", "Geometry" };
const int Spirver::stageCount = 3;
std::stringstream Spirver::errors = std::stringstream();

const char* Spirver::logTypeStr[] = { "Program", "Shader", "PrespecShader" };

GLuint Spirver::StageToGlsl(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return GL_VERTEX_SHADER;
	case Stage::Fragment: return GL_FRAGMENT_SHADER;
	case Stage::Geometry: return GL_GEOMETRY_SHADER;
	case Stage::TessellationControl: return GL_TESS_CONTROL_SHADER;
	case Stage::TessellationEvaluation: return GL_TESS_EVALUATION_SHADER;
	case Stage::Compute: return GL_COMPUTE_SHADER;
	default: return GL_VERTEX_SHADER;
	}
}

EShLanguage Spirver::StageToGlslang(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return EShLanguage::EShLangVertex;
	case Stage::Fragment: return EShLanguage::EShLangFragment;
	case Stage::Geometry: return EShLanguage::EShLangGeometry;
	case Stage::TessellationControl: return EShLanguage::EShLangTessControl;
	case Stage::TessellationEvaluation: return EShLanguage::EShLangTessEvaluation;
	case Stage::Compute: return EShLanguage::EShLangCompute;
	default: return EShLanguage::EShLangVertex;
	}
}

int Spirver::StageToInt(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return 0;
	case Stage::Fragment: return 1;
	case Stage::Geometry: return 2;
	case Stage::TessellationControl: return 3;
	case Stage::TessellationEvaluation: return 4;
	case Stage::Compute: return 5;
	default: return 0;
	}
}

Spirver::Stage Spirver::StageToSpirver(GLenum stage)
{
	switch (stage)
	{
	case GL_VERTEX_SHADER: return Stage::Vertex;
	case GL_FRAGMENT_SHADER: return Stage::Fragment;
	case GL_GEOMETRY_SHADER: return Stage::Geometry;
	case GL_TESS_CONTROL_SHADER: return Stage::TessellationControl;
	case GL_TESS_EVALUATION_SHADER: return Stage::TessellationEvaluation;
	case GL_COMPUTE_SHADER: return Stage::Compute;
	default: return Stage::Vertex;
	}
}

Spirver::Stage Spirver::StageToSpirver(EShLanguage stage)
{
	switch (stage)
	{
	case EShLanguage::EShLangVertex: return Stage::Vertex;
	case EShLanguage::EShLangFragment: return Stage::Fragment;
	case EShLanguage::EShLangGeometry: return Stage::Geometry;
	case EShLanguage::EShLangTessControl: return Stage::TessellationControl;
	case EShLanguage::EShLangTessEvaluation: return Stage::TessellationEvaluation;
	case EShLanguage::EShLangCompute: return Stage::Compute;
	default: return Stage::Vertex;
	}
}

Spirver::Stage Spirver::StageToSpirver(df::detail::SHADER_TYPE stage)
{
	switch (stage)
	{
	case df::detail::SHADER_TYPE::VERTEX: return Stage::Vertex;
	case df::detail::SHADER_TYPE::FRAGMENT: return Stage::Fragment;
	case df::detail::SHADER_TYPE::GEOMETRY: return Stage::Geometry;
	case df::detail::SHADER_TYPE::TESS_CTRL: return Stage::TessellationControl;
	case df::detail::SHADER_TYPE::TESS_EVAL: return Stage::TessellationEvaluation;
	case df::detail::SHADER_TYPE::COMPUTE: return Stage::Compute;
	default: return Spirver::Stage::Vertex;
	}
}

Spirver::Stage Spirver::StageToSpirver(int i)
{
	switch (i)
	{
	case 0: return Stage::Vertex;
	case 1: return Stage::Fragment;
	case 2: return Spirver::Stage::Geometry;
	case 3: return Stage::TessellationControl;
	case 4: return Stage::TessellationEvaluation;
	case 5: return Stage::Compute;
	default: return Stage::Vertex;
	}
}
Spirver::Stage Spirver::StageToSpirver(const std::string& s)
{
	if (s == "vert") return Stage::Vertex;
	else if (s == "frag") return Stage::Fragment;
	else if (s == "geom") return Stage::Geometry;
	else if (s == "tesc") return Stage::TessellationControl;
	else if (s == "tese") return Stage::TessellationEvaluation;
	else if (s == "comp") return Stage::Compute;
	else return Stage::Vertex;
}


std::string Spirver::GetErrors()
{
	std::string ret = errors.str();
	errors = std::stringstream();
	return ret;
}


bool Spirver::printLog(GLuint object, LogType logType)
{
	GLint result = GL_FALSE;
	int logLength = 100;

	switch (logType)
	{
	case LogType::Program:
		glGetProgramiv(object, GL_LINK_STATUS, &result);
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	case LogType::Shader:
		glGetShaderiv(object, GL_COMPILE_STATUS, &result);
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	case LogType::PrespecShader:
		glGetShaderiv(object, GL_SPIR_V_BINARY, &result);
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	}
	
	if (!result)
	{
		if (logLength > 0)
		{
			char* strInfoLog = new char[logLength];
			switch (logType)
			{
			case LogType::Program:
				glGetProgramInfoLog(object, logLength, NULL, strInfoLog);
				break;
			case LogType::Shader:
			case LogType::PrespecShader:
				glGetShaderInfoLog(object, logLength, NULL, strInfoLog);
				break;
			}

			std::stringstream fullMessage;
			fullMessage << logTypeStr[(int)logType] << " error:" << std::endl << strInfoLog;

			errors << fullMessage.str() << std::endl;
			ASSERT(true, fullMessage.str().c_str());

			delete[] strInfoLog;
		}
		else
		{
			std::stringstream fullMessage;
			fullMessage << logTypeStr[(int)logType] << " error!";

			errors << fullMessage.str() << std::endl;
			ASSERT(true, fullMessage.str().c_str());
		}

		return false;
	}
	else return true;
}

bool Spirver::printLog(glslang::TShader* object)
{
	std::string log(object->getInfoLog());
	if (log.size() > 0)
	{
		std::stringstream fullMessage;
		fullMessage << "AST shader error: " << std::endl << log;

		errors << fullMessage.str() << std::endl;
		ASSERT(true, fullMessage.str().c_str());

		return false;
	}
	return true;
}

bool Spirver::printLog(glslang::TProgram* object)
{
	std::string log(object->getInfoLog());
	if (log.size() > 0)
	{
		std::stringstream fullMessage;
		fullMessage << "AST program error: " << std::endl << log;

		errors << fullMessage.str() << std::endl;
		ASSERT(true, fullMessage.str().c_str());

		return false;
	}
	return true;
}

bool Spirver::printLog(spv::SpvBuildLogger& object)
{
	std::string log = object.getAllMessages();
	if (log.length() > 0)
	{
		std::stringstream fullMessage;
		fullMessage << "SPIR-V compile error: " << std::endl << log;

		errors << fullMessage.str() << std::endl;
		ASSERT(true, fullMessage.str().c_str());

		return false;
	}
	return true;
}

void Spirver::printSpirvOptLog(spv_message_level_t level, const char* source,
	const spv_position_t& position, const char* msg)
{
	std::stringstream fullMessage;
	switch (level) {
	case SPV_MSG_FATAL:
	case SPV_MSG_INTERNAL_ERROR:
	case SPV_MSG_ERROR:
		fullMessage << "Spir-V optimizer error at line " << position.index << ": " << msg;

		errors << fullMessage.str() << std::endl;
		ASSERT(true, fullMessage.str().c_str());
		break;
	case SPV_MSG_WARNING:
		fullMessage << "Spir-V optimizer warning at line " << position.index << ": " << msg;

		WARNING(true, fullMessage.str().c_str());
		break;
	case SPV_MSG_INFO:
		fullMessage << "Spir-V optimizer info at line " << position.index << ": " << msg;

		WARNING(true, fullMessage.str().c_str());
		break;
	default:
		break;
	}
}

void Spirver::Init()
{
	glslang::InitializeProcess();
}

void Spirver::Clean()
{
	glslang::FinalizeProcess();
}


bool Spirver::glslToSpirv(const std::string& glsl, std::vector<GLuint>& spirv, Stage stage, int uniformBase, bool analyze)
{
	glslang::TShader* astshader = new glslang::TShader(StageToGlslang(stage));
	if (uniformBase >= 0) { if (!glslToAstShader(glsl, astshader, uniformBase)) return false; }
	else { if (!glslToAstShader(glsl, astshader)) return false; }

	glslang::TProgram* astprogram = new glslang::TProgram();
	if(!astShaderToAstProgram(astshader, astprogram)) return false;

	bool ret = astProgramToSpirv(astprogram, spirv, stage);
	delete astprogram; delete astshader;
	return ret;
}

bool Spirver::glslToAstShader(const std::string& source, glslang::TShader* shader, int uniformBase)
{
	const char* const s[] = { source.c_str() };
	shader->setStrings(s, 1);
	shader->setEnvInput(glslang::EShSource::EShSourceGlsl, shader->getStage(), glslang::EShClient::EShClientOpenGL, 460);
	shader->setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetClientVersion::EShTargetOpenGL_450);
	shader->setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
	if (uniformBase > -1)
	{
		shader->setUniformLocationBase(uniformBase);
		shader->setAutoMapLocations(true);
	}

	shader->parse(&DefaultTBuiltInResource, 110, false, EShMessages::EShMsgSpvRules);
	return printLog(shader);
}

bool Spirver::astShaderToAstProgram(glslang::TShader* shader, glslang::TProgram* program)
{
	program->addShader(shader);
	program->link(EShMessages::EShMsgDefault);
	return printLog(program);
}

bool Spirver::astProgramToSpirv(glslang::TProgram* program, std::vector<GLuint>& spirv, Stage stage)
{
	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;
	spvOptions.disableOptimizer = true;
	glslang::GlslangToSpv(*(program->getIntermediate(StageToGlslang(stage))), spirv, &logger, &spvOptions);
	return printLog(logger);
}


const TBuiltInResource Spirver::DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,
	/* .maxDualSourceDrawBuffersEXT = */ 1,

	/* .limits = */ {
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	} };