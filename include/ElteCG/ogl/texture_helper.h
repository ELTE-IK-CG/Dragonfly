#pragma once
#include "object.h"
#include <cstdint>

//namespace for opengl base classes
namespace eltecg { namespace ogl { namespace helper {
	
/****************************************************************************
 *						Texture helper functions							*	
 ****************************************************************************/
 
 
	//Returns the most fitting internalFormat for the number of cannels and bytes per channel
	//Whenever available, the floating point version is returned
	GLuint getInternalFormat(int num_of_channels, int bytes_per_channel = 8);
	
	//Returns base format for a given internalFormat
	GLuint getBaseFormat(GLuint internalFormat);
	
	//Returns base type for a given internalFormat
	GLuint getBaseType(GLuint internalFormat);

/****************************************************************************
 *						Template functions									*/	
	
	template<typename T_type>
	constexpr GLenum getOglType()
	{	static_assert(false,"Invalid type.");	}
	
	template<> constexpr GLenum getOglType<float>() { return GL_FLOAT; }
	template<> constexpr GLenum getOglType<int8_t>() { return GL_BYTE; }
	template<> constexpr GLenum getOglType<int16_t>() { return GL_SHORT; }
	template<> constexpr GLenum getOglType<int32_t>() { return GL_INT; }
	template<> constexpr GLenum getOglType<uint8_t>() { return GL_UNSIGNED_BYTE; }
	template<> constexpr GLenum getOglType<uint16_t>() { return GL_UNSIGNED_SHORT; }
	template<> constexpr GLenum getOglType<uint32_t>() { return GL_UNSIGNED_INT; }
	
	template<typename ... T_formats>
	constexpr GLuint getInternalFormat()
	{	return getInternalFormatImpl<T_formats ...>();	}
	
/****************************************************************************
 *						Helper types										*/	
 
	template<int T_precision> struct floating {};
	template<int T_precision> struct signed_integral {};
	template<int T_precision> struct unsigned_integral {};
	//TODO: unnormalized_singed_integral<>...
	//TODO: unnormalized_unsinged_integral<>...
	using half_float = floating<16>;

	
/****************************************************************************
 *						Convert common types								*/	
 
	
	//convert floating<32> to float, signed_integral<n> to intn_t, unsigned_integral<n> to uint_t.
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< floating<32>, T_tail_formats ... >()
	{	return getInternalFormat<float,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<8>, T_tail_formats ... >()
	{	return getInternalFormat<int8_t,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<16>, T_tail_formats ... >()
	{	return getInternalFormat<int16_t,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<32>, T_tail_formats ... >()
	{	return getInternalFormat<int32_t,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< unsigned_integral<8>, T_tail_formats ... >()
	{	return getInternalFormat<uint8_t,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< unsigned_integral<16>, T_tail_formats ... >()
	{	return getInternalFormat<uint16_t,  T_tail_formats ... >();	}
	
	template<typename .. T_tail_formats>
	constexpr GLuint getInternalFormat< unsigned_integral<32>, T_tail_formats ... >()
	{	return getInternalFormat<uint32_t,  T_tail_formats ... >();	}
	
	template<typename ... T_formats>
	constexpr GLuint getInternalFormatImpl()
	{
		static_assert(false, "No OpenGL format corresponts to given type(s).");
	}

	
/****************************************************************************
 *						Choose format										*/	
 	

//shorten following definitions
#define T_GET_INT_FORMAT template<> constexpr GLuint getInternalFormatImpl

//Unsinged normalized integer formats
	T_GET_INT_FORMAT<uint8_t>() { return GL_R8; }
	T_GET_INT_FORMAT<uint8_t, uint8_t>() { return GL_RG8; }
	T_GET_INT_FORMAT<uint8_t, uint8_t, uint8_t>() { return G_RGB8; }
	T_GET_INT_FORMAT<uint8_t, uint8_t, uint8_t, uint8_t>() { return G_RGBA8; }

	T_GET_INT_FORMAT<uint16_t> { return GL_R16; }
	T_GET_INT_FORMAT<uint16_t, uint16_t> { return GL_RG16; }
	T_GET_INT_FORMAT<uint16_t, uint16_t, uint16_t> { return G_RGB16; }
	T_GET_INT_FORMAT<uint16_t, uint16_t, uint16_t, uint16_t> { return G_RGBA16; }

	T_GET_INT_FORMAT<uint32_t> { return GL_R32; }
	T_GET_INT_FORMAT<glm::uvec2> { return GL_RG32; }
	T_GET_INT_FORMAT<uint32_t, uint32_t> { return GL_RG32; }
	T_GET_INT_FORMAT<glm::uvec3> { return G_RGB32; }
	T_GET_INT_FORMAT<uint32_t, uint32_t, uint32_t> { return G_RGB32; }
	T_GET_INT_FORMAT<glm::uvec4> { return G_RGBA32; }
	T_GET_INT_FORMAT<uint32_t, uint32_t, uint32_t, uint32_t> { return G_RGBA32; }
	
//Signed normalized integer formats
	T_GET_INT_FORMAT<int8_t> { return GL_R8_SNORM; }
	T_GET_INT_FORMAT<int8_t, int8_t> { return GL_RG8_SNORM; }
	T_GET_INT_FORMAT<int8_t, int8_t, int8_t> { return G_RGB8_SNORM; }
	T_GET_INT_FORMAT<int8_t, int8_t, int8_t, int8_t> { return G_RGBA8_SNORM; }

	T_GET_INT_FORMAT<int16_t> { return GL_R16_SNORM; }
	T_GET_INT_FORMAT<int16_t, int16_t> { return GL_RG16_SNORM; }
	T_GET_INT_FORMAT<int16_t, int16_t, int16_t> { return G_RGB16_SNORM; }
	T_GET_INT_FORMAT<int16_t, int16_t, int16_t, int16_t> { return G_RGBA16_SNORM; }

	T_GET_INT_FORMAT<int32_t> { return GL_R32_SNORM; }
	T_GET_INT_FORMAT<glm::ivec2> { return GL_RG32_SNORM; }
	T_GET_INT_FORMAT<int32_t, int32_t> { return GL_RG32_SNORM; }
	T_GET_INT_FORMAT<glm::ivec3> { return G_RGB32_SNORM; }
	T_GET_INT_FORMAT<int32_t, int32_t, int32_t> { return G_RGB32_SNORM; }
	T_GET_INT_FORMAT<glm::ivec4> { return G_RGBA32_SNORM; }
	T_GET_INT_FORMAT<int32_t, int32_t, int32_t, int32_t> { return G_RGBA32_SNORM; }
	
//Floating integer formats
	T_GET_INT_FORMAT<half_float>() { return GL_R16F; }
	T_GET_INT_FORMAT<half_float, half_float>() { return GL_RG16F; }
	T_GET_INT_FORMAT<half_float, half_float, half_float>() { return GL_RGB16F; }
	T_GET_INT_FORMAT<half_float, half_float, half_float, half_float>() { return GL_RGBA16F; }
	
	T_GET_INT_FORMAT<float>() { return GL_R32F; }
	T_GET_INT_FORMAT<float, float>() { return GL_RG32F; }
	T_GET_INT_FORMAT<glm::vec2>() { return GL_RG32F; }
	T_GET_INT_FORMAT<float, float, float>() { return GL_RGB32F; }
	T_GET_INT_FORMAT<glm::vec3>() { return GL_RGBA32F; }
	T_GET_INT_FORMAT<float, float, float, float>() { return GL_RGBA32F; }
	T_GET_INT_FORMAT<glm::vec4>() { return GL_RGBA32F; }

//Uncommon unsinged integer formats

	T_GET_INT_FORMAT<unsigned_integral<4>, unsigned_integral<4>, unsigned_integral<4>>() { return GL_RGB4; }
	T_GET_INT_FORMAT<unsigned_integral<5>, unsigned_integral<5>, unsigned_integral<5>>() { return GL_RGB5; }
	T_GET_INT_FORMAT<unsigned_integral<10>, unsigned_integral<10>, unsigned_integral<10>>() { return GL_RGB10; }
	T_GET_INT_FORMAT<unsigned_integral<12>, unsigned_integral<12>, unsigned_integral<12>>() { return GL_RGB12; }
	
	T_GET_INT_FORMAT<unsigned_integral<2>, unsigned_integral<2>, unsigned_integral<2>, unsigned_integral<2>>() { return GL_RGBA2; }
	T_GET_INT_FORMAT<unsigned_integral<4>, unsigned_integral<4>, unsigned_integral<4>, unsigned_integral<4>>() { return GL_RGBA4; }
	T_GET_INT_FORMAT<unsigned_integral<12>, unsigned_integral<12>, unsigned_integral<12>, unsigned_integral<12>>() { return GL_RGBA12; }

//Special types
	 //TODO: finish!
	 //source: Table 1 at https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	
#undef T_GET_INT_FORMAT	
	
} } } // namespace eltecg::ogl::helper