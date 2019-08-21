#pragma once
#include "object.h"
#include <vector>	//fwd decl?
#include <map>		//fwd decl?
//TODO: pimpl!!

//namespace for opengl classes
namespace eltecg { namespace ogl {


/****************************************************************************
 *						OpenGL Program class									*
 ****************************************************************************/


class Program final
	: public Object
{
public:

// maybe we don't need this assert macro:
#define PROGRAM_BIND_ASSERT ASSERT(this->object_id == this->s_bound_program, "Object not bound to the context.")


/****************************************************************************
 *						Constructors and destructors						*/
 
	Program() { this->object_id = glCreateProgram() }
	~Program() { glDeleteProgram(this->object_id); }
	
	void saveCache(char *filename); //TODO
	void loadCache(char *filename); //TODO

/****************************************************************************
 *						Binding												*/

	inline void bindProgram()
	{
		if(this->s_bound_program != this->object_id)
		{
			glUseProgram(this->object_id);
			this->s_bound_program = this->object_id;
		}
	}

	// TODO: setUniform functions
	void setUniform(const char * uniform_vname, int i);
	
	//Following should also be in setup section :D
	void bindAttribLoc(int _index, const char* _uniform);
	void bindFragDataLoc(int _index, const char* _uniform);
	
/****************************************************************************
 *						Setup												*/
	
	//TODO: already implemented functions
	bool linkProgram(); //also prints errors

	template<ShaderType T_shader_type>
	void attachShader(const Shader<T_shader_type> &shader)
	{
		constexpr int index = shaderTypeToIndex<T_shader_type>();
		ASSERT(this->m_program_attached_shaders[index] != static_cast<GLuint>(shader),
			"This shader is already attached.");
		//Otherwise shader already attached. (TODO: What if shader recompiled?)
		glAttachShader(this->object_id, static_cast<GLuint>(shader));
		this->m_program_attached_shaders[index] = shader.get_object_id();
		//NOTE:	OpenGL specification supports multiple shaders attached of the same kind with only of them having a main() function
		//		However, we do not support this because driver implementations don't usually work well with this effect.
	}
	
	template<ShaderType T_shader_type>
	void dettachShader(const Shader<T_shader_type> &shader)
	{
		constexpr int index = shaderTypeToIndex<T_shader_type>();
		ASSERT(this->m_program_attached_shaders[index] == static_cast<GLuint>(shader),
			"This is not the shader attached, thus it cannot be dettached.");
		glDettachShader(this->object_id, static_cast<GLuint>(shader));
		this->m_program_attached_shaders[index] = static_cast<GLuint>(shader);
	}
	template<ShaderType T_shader_type>
	void dettachShader()
	{
		constexpr int index = shaderTypeToIndex<T_shader_type>();
		ASSERT(this->m_program_attached_shaders[index] != 0, "No shader attached here");
		glDettachShader(this->object_id, static_cast<GLuint>(shader));
		this->m_program_attached_shaders[index] = static_cast<GLuint>(shader);
	}

protected:

/****************************************************************************
 *						Utility												*/
	
	template<ShaderType T_shader_type> constexpr int shaderTypeToIndex()
	{
		static_assert(false, "invalid shader type");
	}
	template<> constexpr int shaderTypeToIndex<ShaderType::VERTEX_SHADER		 >() { return 0;}
	template<> constexpr int shaderTypeToIndex<ShaderType::TESS_CONTROL_SHADER	 >() { return 1;}
	template<> constexpr int shaderTypeToIndex<ShaderType::TESS_EVALUATION_SHADER>() { return 2;}
	template<> constexpr int shaderTypeToIndex<ShaderType::GEOMETRY_SHADER		 >() { return 3;}
	template<> constexpr int shaderTypeToIndex<ShaderType::FRAGMENT_SHADER		 >() { return 4;}
	template<> constexpr int shaderTypeToIndex<ShaderType::COMPUTE_SHADER		 >() { return 5;}
	
	template<int T_index> constexpr int indexToShaderType()
	{
		static_assert(false, "invalid index");
	}
	template<> constexpr int shaderTypeToIndex<0>() { return ShaderType::VERTEX_SHADER			;}
	template<> constexpr int shaderTypeToIndex<1>() { return ShaderType::TESS_CONTROL_SHADER	;}
	template<> constexpr int shaderTypeToIndex<2>() { return ShaderType::TESS_EVALUATION_SHADE	;}
	template<> constexpr int shaderTypeToIndex<3>() { return ShaderType::GEOMETRY_SHADER		;}
	template<> constexpr int shaderTypeToIndex<4>() { return ShaderType::FRAGMENT_SHADER		;}
	template<> constexpr int shaderTypeToIndex<5>() { return ShaderType::COMPUTE_SHADER			;}
	
protected:

/****************************************************************************
 *						Variables											*/
 
	//TODO: key should be the hash of the string. There is no need for holding
	std::map<std::string, GLint>	m_program_uniform_locations;
	
	GLuint m_program_attached_shaders[6] = {0,0,0,0,0,0};
	
	static GLuint s_bound_program = 0;

#undef PROGRAM_BIND_ASSERT
	
	//Pipeline will be a friend
	
}; // Program class

} } //namespace eltecg::ogl