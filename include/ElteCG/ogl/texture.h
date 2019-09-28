#pragma once
#include "object.h"

//namespace for opengl base classes
namespace eltecg { namespace ogl {
	
/****************************************************************************
 *						Enums												*/	

 enum class TextureType : decltype(GL_TEXTURE_1D)
{
	TEX_1D						= GL_TEXTURE_1D						,
	TEX_2D						= GL_TEXTURE_2D						,
	TEX_3D						= GL_TEXTURE_3D						,
	TEX_RECTANGLE				= GL_TEXTURE_RECTANGLE				,
	TEX_BUFFER					= GL_TEXTURE_BUFFER					,
	TEX_CUBE_MAP				= GL_TEXTURE_CUBE_MAP				,
	TEX_1D_ARRAY				= GL_TEXTURE_1D_ARRAY				,
	TEX_2D_ARRAY				= GL_TEXTURE_2D_ARRAY				,
	TEX_CUBE_MAP_ARRAY			= GL_TEXTURE_CUBE_MAP_ARRAY			,
	TEX_2D_MULTISAMPLE			= GL_TEXTURE_2D_MULTISAMPLE			,
	TEX_2D_MULTISAMPLE_ARRAY	= GL_TEXTURE_2D_MULTISAMPLE_ARRAY	,
};

enum class TextureSwizzleComponent : decltype(GL_RED)
{
	RED		= GL_RED	,		GREEN	= GL_GREEN	,
	BLUE	= GL_BLUE	,		ALPHA	= GL_ALPHA	,
	ZERO	= GL_ZERO	,		ONE		= GL_ONE	,
};

enum class TextureInterpolation 
{
	NONE = 0,
	NEAREST = 1,
	LINEAR = 2,
};

enum class TextureWrapMode : decltype(GL_CLAMP_TO_BORDER)
{
	CLAMP_TO_BORDER       = GL_CLAMP_TO_BORDER      ,
	CLAMP_TO_EDGE         = GL_CLAMP_TO_EDGE        ,
	MIRRORED_REPEAT       = GL_MIRRORED_REPEAT      ,
	REPEAT				  = GL_REPEAT			    ,      
#if OPENGL_VERSION >= 44
	MIRROR_CLAMP_TO_EDGE  = GL_MIRROR_CLAMP_TO_EDGE ,
#endif
};

/****************************************************************************
 *						OpenGL Texture base class							*
 ****************************************************************************/

//https://www.khronos.org/opengl/wiki/Texture
template<TextureType T_texture_type>
	class Texture
		: public Object
{
	
#define TEXTURE_BIND_ASSERT ASSERT(this->object_id == this->s_bound_texture_id, "Object not bound to the context.")

public:
/****************************************************************************
 *						Constructors and destructors						*/
	Texture()
	{
		static_assert( T_texture_type == TextureType::TEX_1D					
			|| T_texture_type == TextureType::TEX_2D					
			|| T_texture_type == TextureType::TEX_3D					
			|| T_texture_type == TextureType::TEX_RECTANGLE			
			|| T_texture_type == TextureType::TEX_BUFFER				
			|| T_texture_type == TextureType::TEX_CUBE_MAP			
			|| T_texture_type == TextureType::TEX_1D_ARRAY			
			|| T_texture_type == TextureType::TEX_2D_ARRAY			
			|| T_texture_type == TextureType::TEX_CUBE_MAP_ARRAY		
			|| T_texture_type == TextureType::TEX_2D_MULTISAMPLE		
			|| T_texture_type == TextureType::TEX_2D_MULTISAMPLE_ARRAY, "Invalid texture type");
		glGenTextures(1, &this->object_id);
	}
	Texture(){ glDeleteTextures(1, &this->object_id);}
	
/****************************************************************************
 *						Operators											*/

	void generateMipmaps()
	{
		static_assert( T_texture_type == TextureType::TEX_1D					
			|| T_texture_type == TextureType::TEX_2D					
			|| T_texture_type == TextureType::TEX_3D					
			|| T_texture_type == TextureType::TEX_CUBE_MAP			
			|| T_texture_type == TextureType::TEX_1D_ARRAY			
			|| T_texture_type == TextureType::TEX_2D_ARRAY			
			|| T_texture_type == TextureType::TEX_CUBE_MAP_ARRAY, "Invalid texture type");
#if OPENGL_VERSION >= 45
		glGenerateTextureMipmap(this->object_id); //direct state access
#else
		TEXTURE_BIND_ASSERT;
		glGenerateMipmap(static_cast<GLenum>(T_texture_type));
#endif
	}
 
 
/****************************************************************************
 *						Binding												*/

	void bindTexture()
	{
		if(this->s_bound_texture_id != this->object_id)
		{
			glBindTexture(static_cast<GLenum>(T_texture_type), this->object_id);
			this->s_bound_texture_id = this->object_id;
		}
	}
	
	static void activeTexture(int index) const
	{
	#warning Try using OGLSampler instead
		ASSERT(0 <= index, "Invalid index for texture");
	#if OPENGL_VERSION >= 42
		WARNING(index < 80, "Invalid index in OpenGL 4.2 - 4.4");
	#elif OPENGL_VERSION >= 45
		WARNING(index < 96, "Invalid index in OpenGL 4.5");
	#else
		WARNING(index < 96, "Invalid index in OpenGL 3.3 - 4.1");
	#endif
		//TODO: OpenGL 4.5 : 96, OpenGL 4.2 : 80, OpenGL 3.3 : 48
		TEXTURE_BIND_ASSERT;
		//TODO: If GPU_DEBUG defined : glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0, &max_index);
		static int active_texture = -1;
		if(active_texture != index)
		{
			glActiveTexture(index);
			active_texture = index;
		}
	}

/****************************************************************************
 *						Checks												*/

	//TODO: implement texture completeness checking functions:
	bool checkMipmapCompleteness() const { return true; }
	bool checkCubemapCompleteness() const { return true;}
	bool checkSamplercompleteness(const OGLSampler &sampler) const { return true; }

/****************************************************************************
 *						Setup												*/

	void setToStencilMode()
	{
	#if OPENGL_VERSION >= 45
		glTextureParameteri(this->object_id, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT);
	#else
		TEXTURE_BIND_ASSERT;
		glTexParameteri(static_cast<GLenum>(T_texture_type), GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT);
	#endif
	}
	void setToDepthMode()
	{
	#if OPENGL_VERSION >= 45
		glTextureParameteri(this->object_id, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
	#else
		TEXTURE_BIND_ASSERT;
		glTexParameteri(static_cast<GLenum>(T_texture_type), GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
	#endif
	}
	
	void setMipmapLevelRange(int base = 0, int max = 1000)
	{
		ASSERT(0 <= base && base <= max);
	#if OPENGL_VERSION >= 45
		glTextureParameteri(this->object_id, GL_TEXTURE_BASE_LEVEL, base);
		glTextureParameteri(this->object_id, GL_TEXTURE_BASE_LEVEL, max);
	#else
		TEXTURE_BIND_ASSERT;
		glTexParameteri(static_cast<GLenum>(T_texture_type), GL_TEXTURE_BASE_LEVEL, base);
		glTexParameteri(static_cast<GLenum>(T_texture_type), GL_TEXTURE_MAX_LEVEL, max);
	#endif
	}
	
	void setSwizzleMask(TextureSwizzleComponent R = TextureSwizzleComponent::RED,
		TextureSwizzleComponent G = TextureSwizzleComponent::GREEN,
		TextureSwizzleComponent B = TextureSwizzleComponent::BLUE,
		TextureSwizzleComponent A = TextureSwizzleComponent::ALPHA)
	{
		const GLint arr[] = {R, G, B, A};
	#if OPENGL_VERSION >= 45
		glTextureParameteriv(this->object_id, GL_TEXTURE_SWIZZLE_RGBA, arr);
	#else
		TEXTURE_BIND_ASSERT;
		glTexParameteriv(static_cast<GLenum>(T_texture_type), GL_TEXTURE_SWIZZLE_RGBA, arr);
	#endif
	}
 
/****************************************************************************
 *						Getters												*/

inline GLsizei getWidth() const { return m_width; }
inline GLsizei getHeight() const { return m_width; }
inline GLsizei getDepth() const { return m_width; }
inline GLsizei getLeves() const { return m_width; }
inline GLuint getInternalFormat() const {return m_internalFormat; }
 
protected:

/****************************************************************************
 *						Variables											*/
 
	static GLuint s_bound_texture_id = 0; // the id of the currently bound texture
	
	GLsizei m_width = 0;	// Width of the texture. Unused dimensions have size 0.
	GLsizei m_height = 0;	// Height of the texture. Unused dimensions have size 0.
	GLsizei m_depth = 0;	// Depth of the texture. Unused dimensions have size 0.
	GLsizei m_levels = 0;	// Number of mipmap levels of the texture. It is zero only when the texture does not support it.
	GLuint m_internalFormat = 0;	// Internal format of the texture
	
#undef TEXTURE_BIND_ASSERT

}; //Texture class

} } //namespace eltecg::ogl::base