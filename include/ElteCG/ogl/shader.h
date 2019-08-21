#pragma once
#include "object.h"
#include <vector> //fwd decl?

//namespace for opengl classes
namespace eltecg { namespace ogl {
	
/****************************************************************************
 *						Enums												*/
enum class ShaderType
{
	VERTEX_SHADER			= GL_VERTEX_SHADER			,
	TESS_CONTROL_SHADER		= GL_TESS_CONTROL_SHADER	,
	TESS_EVALUATION_SHADER	= GL_TESS_EVALUATION_SHADER	,
	GEOMETRY_SHADER			= GL_GEOMETRY_SHADER		,
	FRAGMENT_SHADER			= GL_FRAGMENT_SHADER		,
	COMPUTE_SHADER			= GL_COMPUTE_SHADER			,
};

/****************************************************************************
 *						OpenGL Shader class									*
 ****************************************************************************/


template<ShaderType T_shader_type>
	class Shader final
		: public base::Object
{
public:

/****************************************************************************
 *						Constructors and destructors						*/

	Shader()
	{
		static_assert(ShaderType == ShaderType::VERTEX_SHADER				
			|| T_shader_type == ShaderType::TESS_CONTROL_SHADER			
			|| T_shader_type == ShaderType::TESS_EVALUATION_SHADER		
			|| T_shader_type == ShaderType::GEOMETRY_SHADER				
			|| T_shader_type == ShaderType::FRAGMENT_SHADER				
			|| T_shader_type == ShaderType::COMPUTE_SHADER, "Invalid shader type");
		this->object_id = glCreateShader(T_shader_type);
	}
	
	~OGLShader() { glDeleteShader(this->object_id); }


/****************************************************************************
 *						Setup												*/

	//TODO copy paste implementation !!
	bool compileCode(const std::vector<char*> &codes);
	inline bool compileCode(const char* code) { return compileCode(std::vector<char*>({code})); }
	inline bool compileCode(const std::string &code) { return compileCode(code.c_str()); }

};

} } //namespace eltecg::ogl