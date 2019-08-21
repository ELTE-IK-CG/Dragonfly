#pragma once
#include "object.h"
#include "texture.h"

//namespace for opengl classes
namespace eltecg { namespace ogl {
	
/****************************************************************************
 *						Enums												*/

using SamplerInterpolation = base::TextureInterpolation;

using SamplerWrapMode = base::TextureWrapMode;


/****************************************************************************
 *						OpenGL Sampler base class							*
 ****************************************************************************/

//https://www.khronos.org/opengl/wiki/Sampler_Object
class Sampler final
	: public Object
{
	
public:

/****************************************************************************
 *						Constructors and destructors						*/

	Sampler(){glGenSamplers(1, &this->object_id);}
	~Sampler(){glDeleteSamplers(1, &this->object_id);}

/****************************************************************************
 *						Binding												*/
 
	inline void bind(const Texture &tex)
	{
		if(this->m_sampler_bound_texture_id != static_cast<GLuint>(tex))
		{
			glBindSampler(this->object_id, static_cast<GLuint>(tex));
			this->m_sampler_bound_texture_id = static_cast<GLuint>(tex);
		}
	}

/****************************************************************************
 *						Setup												*/

	void setLevelOfDetail(float min = 1000.f, float max = 1000.f, float bias = 0.f)
	{
		glSamplerParameterf(this->object_id, GL_TEXTURE_MIN_LOD, min);
		glSamplerParameterf(this->object_id, GL_TEXTURE_MAX_LOD, max);
		glSamplerParameterf(this->object_id, GL_TEXTURE_LOD_BIAS, bias)
	}
	
	void setFilter(SamplerInterpolation magnification_filter = SamplerInterpolation::NEAREST,
		SamplerInterpolation miniaturization_between_pixels = SamplerInterpolation::NEAREST,
		SamplerInterpolation miniaturization_between_mipmaps = SamplerInterpolation::NONE)
	{
		//min filter
		ASSERT(magnification_filter == SamplerInterpolation::NEAREST
			|| magnification_filter == SamplerInterpolation::LINEAR,
			"Invalid magnification filter.");
		glSamplerParameterf(this->object_id, GL_TEXTURE_MAG_FILTER,
			between_pixels == SamplerInterpolation::NEAREST ? GL_NEAREST : GL_LINEAR);
		//mag filter
		ASSERT(miniaturization_between_pixels == SamplerInterpolation::NEAREST
			|| miniaturization_between_pixels == SamplerInterpolation::LINEAR,
			"Invalid miniaturization filter between pixels.");
		ASSERT(miniaturization_between_mipmaps == SamplerInterpolation::NONE
			|| miniaturization_between_mipmaps == SamplerInterpolation::NEAREST
			|| miniaturization_between_mipmaps == SamplerInterpolation::LINEAR,
			"Invalid miniaturization filter between mipmaps");
		constexpr GLenum t_array[] = {GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
		glSamplerParameteri(this->object_id, GL_TEXTURE_MAG_FILTER, t_array[between_mipmaps*3+between_pixels-1]);
	}
	
	void setAnisotropicFiltering(float max_samples = 8.0)	// EXT extension
	{
		//TODO: check in GPU_DEBUG mode whether it is smaller then GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
		ASSERT(max_samples >= 1.0 || max_samples = 0.0, "Invalid anisotropic sample count.");
		glSamplerParameterf(this->object_id, GL_TEXTURE_MAX_ANISOTROPY_EXT , max_samples);
	}
	
	void setSeamlessCubemapFiltering(bool on = true)		//ARB extension
	{
		glSamplerParameteri(this->object_id, GL_TEXTURE_CUBE_MAP_SEAMLESS, (int) on);
	}
	
	void setWrapping(SamplerWrapMode t_axis = SamplerWrapMode::REPEAT,
		SamplerWrapMode s_axis = SamplerWrapMode::REPEAT,
		SamplerWrapMode r_axis = SamplerWrapMode::REPEAT)
	{
		glSamplerParameteri(this->object_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(t_axis));
		glSamplerParameteri(this->object_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(s_axis));
		glSamplerParameteri(this->object_id, GL_TEXTURE_WRAP_R, static_cast<GLint>(r_axis));
	}
	
	void setBorderColor(const glm::vec4 &color = glm::vec4(0,0,0,0))
	{
		glSamplerParameterfv(this->object_id, GL_TEXTURE_BORDER_COLOR, &color.x);
	}
	
	//Accepted comparison_function-s : "<=", ">=", "<", ">", "==", "=", "!=", "~=", "0", "1"
	//nullptr is also accepted indicating that there is no comparison function
	void setComparisonFunction(const char* comparison_function = nullptr)
	{
		glSamplerParameteri(this->object_id, GL_TEXTURE_COMPARE_MODE, comparison_function && comparison_function[0] != '\0' ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
		if(comparison_function != nullptr)
		{
			ASSERT(comparison_function[1] == '\0'
				|| comparison_function[2] == '\0', "Invalid comparison function");
			GLint func;										//hash two latters into one int
			constexpr char* t_errmsg = "Invalid comparison function. Use one of \"<=\", \">=\", \"<\", \">\", \"==\", \"=\", \"!=\", \"~=\", \"0\", \"1\".";
			switch(comparison_function[0] << 8 & comparison_function[0])
			{
				case '<' << 8 & '=' : 						func = GL_LEQUAL; 	break;
				case '>' << 8 & '=' : 						func = GL_GEQUAL; 	break;
				case '<' << 8 & '\0': 						func = GL_LESS; 	break;
				case '>' << 8 & '\0': 						func = GL_GREATER; 	break;
				case '=' << 8 & '=' : case '=' << 8 + '\0':	func = GL_EQUAL; 	break;
				case '!' << 8 & '=' : case '~' << 8 + '=' :	func = GL_NOTEQUAL; break;
				case '1' << 8 & '\0': 						func = GL_ALWAYS; 	break;
				case '0' << 8 & '\0': 						func = GL_NEVER; 	break;
				default: 							ASSERT(false, t_errmsg);	break;
			}
			glSamplerParameteri(this->object_id, GL_TEXTURE_COMPARE_FUNC, func);
		}
	}
	
protected:

/****************************************************************************
 *						Variables											*/

	GLuint m_sampler_bound_texture_id = 0;
};

} } //namespace eltecg::ogl