#pragma once
#include "texture.h"
#include "texture_helper.h"

/****************************************************************************
 *						Texture2D class 									*	
 ****************************************************************************/
 

//	TODO Split/inherit class to 3 classes. Mutable, Immutable and Buffer Storage.
class Texture2D final
	: public Texture<TextureType::TEX_2D>
{

#define TEXTURE_BIND_ASSERT ASSERT(this->object_id == this->s_bound_texture_id, "Object not bound to the context.")
	
public:

/****************************************************************************
 *						Storage definition									*/	

	//Immutable storage:
	template<typename ... T_format>
	void allocateTexture(GLsizei width, GLsizei height, GLint levels = 1)
	{
		constexpr GLuint t_internalFormat = helper::getInternalFormat<T_format ...>();
		impl_glTexStorage(width, height, levels, t_internalFormat);
	}
	
	void allocateTexture(GLsizei width, GLsizei height, int num_of_channels, int bytes_per_channel = 8, GLint levels = 1)
	{
		GLuint t_internalFormat = helper::getInternalFormat(num_of_channels, bytes_per_channel);
		impl_glTexStorage(width, height, levels, t_internalFormat);
	}
	
	//Mutable storage
	
	template<typename T_data, typename ... T_format>
	void constructImage(GLsizei width, GLsizei height, const T_data * data,
		GLint level = 0, GLenum format = 0)
	{
		constexpr GLuint t_internalFormat = helper::getInternalFormat<T_format ...>();
		constexpr GLuint t_type = helper::getOglType<T_data>();
		if(format == 0)	format = helper::getBaseFormat(t_internalFormat);
		impl_glTexImage2D(width, height, static_cast<GLvoid*>(data), level, t_internalFormat, format, t_type);
	}
	
	void constructImage(GLsizei width, GLsizei height, const GLvoid * data, 
		int num_of_channels, int bytes_per_channel = 8, GLint level = 0, GLenum format = 0, GLenum type = 0)
	{
		GLuint t_internalFormat = helper::getInternalFormat(num_of_channels, bytes_per_channel);
		if(format == 0)	format = helper::getBaseFormat(t_internalFormat);
		if(type == 0)	type= helper::getBaseType(t_internalFormat);
		impl_glTexImage2D(width, height, data, level, t_internalFormat, format, type);
	}
	
	template<typename T_data>
	void constructImage(GLsizei width, GLsizei height, const T_data * data, 
		int num_of_channels, int bytes_per_channel = 8, GLint level = 0, GLenum format = 0)
	{
		GLuint t_internalFormat = helper::getInternalFormat(num_of_channels, bytes_per_channel);
		constexpr GLuint t_type = helper::getOglType<T_data>();
		if(format == 0)	format = helper::getBaseFormat(t_internalFormat);
		impl_glTexImage2D(width, height, static_cast<GLvoid*>(data), level, t_internalFormat, format, t_type);
	}
	
	template<typename ... T_format>
	void constructImage(GLsizei width, GLsizei height, const GLvoid * data,
		GLint level = 0, GLenum format = 0, GLenum type = 0)
	{
		constexpr GLuint t_internalFormat = helper::getInternalFormat<T_format ...>();
		if(format == 0)	format = helper::getBaseFormat(t_internalFormat);
		if(type == 0)	type= helper::getBaseType(t_internalFormat);
		impl_glTexImage2D(width, height, data, level, t_internalFormat, format, type);
	}
	
/****************************************************************************
 *						Modify storage										*/
 
	template<typename T_data>
	void assignImage(const T_data * data, GLint level = 0, GLenum format = 0)
	{
		constexpr GLuint t_type = helper::getOglType<T_data>();
		if(format == 0)	format = helper::getBaseFormat(m_internalFormat);
		impl_glTexSubImage2D(m_width, m_height, 0, 0, static_cast<GLvoid*>(data), level, format, type);
	}
	
	void assignImage(const GLvoid * data, GLint level = 0, GLenum format = 0, GLenum type = 0)
	{
		if(format == 0)	format = helper::getBaseFormat(m_internalFormat);
		if(type == 0)	type= helper::getBaseType(m_internalFormat);
		impl_glTexSubImage2D(m_width, m_height, 0, 0, data, level, format, type);
	}
	
	template<typename T_data>
	void assignSubImage(GLsizei width, GLsizei height, GLint xoffset, GLint yoffset,
		const T_data * data, GLint level = 0, GLenum format = 0)
	{
		constexpr GLuint t_type = helper::getOglType<T_data>();
		if(format == 0)	format = helper::getBaseFormat(m_internalFormat);
		impl_glTexSubImage2D(width, height, xoffset, yoffset, static_cast<GLvoid*>(data), level, format, type);
	}
	
	void assignSubImage(GLsizei width, GLsizei height, GLint xoffset, GLint yoffset,
		const GLvoid * data, GLint level = 0, GLenum format = 0, GLenum type = 0)
	{
		if(format == 0)	format = helper::getBaseFormat(m_internalFormat);
		if(type == 0)	type= helper::getBaseType(m_internalFormat);
		impl_glTexSubImage2D(width, height, xoffset, yoffset, data, level, format, type);
	}
	
protected:

/****************************************************************************
 *						Helper functions									*/	


	inline void impl_glTexStorage(GLsizei width, GLsizei height, GLint levels, GLuint internalFormat)
	{
		TEXTURE_BIND_ASSERT;
		ASSERT(width >= 1 && height >= 1 && levels >= 1 && levels <= log2(std::max(width, height)), "Invalid dimensions for a 2D texture");

		glTexStorage2D(GL_TEXTURE_2D, levels, internalFormat, width, height);

		m_width = width;
		m_height = height;
		m_levels = levels;
		m_internalFormat = internalFormat;
	}

	inline void impl_glTexImage2D(GLsizei width, GLsizei height, const GLvoid * data,
			GLint level, GLuint internalFormat, GLenum format, GLenum type)
	{
		TEXTURE_BIND_ASSERT;
		ASSERT(width >= 1 && height >= 1 && level >= 0 && level < log2(std::max(width, height)), "Invalid dimensions for a 2D texture");
		
		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, type, data);
		
		m_width = width;
		m_height = height;
		m_levels = std::max(level + 1, m_levels);
		m_internalFormat = internalFormat;
	}
	
	inline void impl_glTexSubImage2D(GLsizei width, GLsizei height, GLint xoffset, GLint yoffset,
		const GLvoid * data, GLint level, GLenum format, GLenum type)
	{
		
		ASSERT(width >= 1 && height >= 1 && level >= 0 && level < log2(std::max(width, height)), "Invalid dimensions for a 2D texture");
		ASSERT(xoffset >= 0 && yoffset >= 0 && xoffset + width <= m_width && yoffset + height <= m_height, "Invalid sub-image");
		
		TEXTURE_BIND_ASSERT;
		glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, data);
	}
	
#undef TEXTURE_BIND_ASSERT

}