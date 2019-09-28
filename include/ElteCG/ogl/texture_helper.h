#pragma once
#include "object.h"
#include <glm/fwd.hpp>
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
	using half = floating<16>;

	
/****************************************************************************
 *						Convert common types								*/	
 
	
	//convert floating<32> to float, signed_integral<n> to intn_t, unsigned_integral<n> to uint_t.
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< floating<32>, T_tail_formats ... >()
	{	return getInternalFormat<float,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<8>, T_tail_formats ... >()
	{	return getInternalFormat<int8_t,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<16>, T_tail_formats ... >()
	{	return getInternalFormat<int16_t,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< signed_integral<32>, T_tail_formats ... >()
	{	return getInternalFormat<int32_t,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< unsigned_integral<8>, T_tail_formats ... >()
	{	return getInternalFormat<uint8_t,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
	constexpr GLuint getInternalFormat< unsigned_integral<16>, T_tail_formats ... >()
	{	return getInternalFormat<uint16_t,  T_tail_formats ... >();	}
	
	template<typename ... T_tail_formats>
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


template<typename T> struct _GetInternalFormat { static constexpr GLenum Get() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return 0; } };

#define DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(cpp_type, gl_type) template<> struct _GetInternalFormat<cpp_type> { static constexpr GLenum Get() { return gl_type; } };

//[0,1] normalized formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint8_t, GL_R8)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec1, GL_R8)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec2, GL_RG8)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec3, GL_RGB8)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec4, GL_RGBA8)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint16_t, GL_R16)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec1, GL_R16)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec2, GL_RG16)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec3, GL_RGB16)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec4, GL_RGBA16)

//[-1,1] normalized formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(int8_t, GL_R8_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec1, GL_R8_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec2, GL_RG8_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec3, GL_RGB8_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec4, GL_RGBA8_SNORM)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(int16_t, GL_R16_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec1, GL_R16_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec2, GL_RG16_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec3, GL_RGB16_SNORM)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec4, GL_RGBA16_SNORM)

// floating point formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half, GL_R16F)
//DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(hvec1, GL_R16F)
//DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(hvec2, GL_RG16F)
//DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(hvec3, GL_RGB16F)
//DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(hvec4, GL_RGBA16F)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(float, GL_R32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec1, GL_R32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec2, GL_RG32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec3, GL_RGB32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec4, GL_RGBA32F)

//TODO FINISH
//32 bit integer formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint32_t, GL_R32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uvec1, GL_R32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uvec2, GL_RG32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uvec3, GL_RGB32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uvec4, GL_RGBA32UI)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(int32_t, GL_R32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::ivec1, GL_R32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::ivec2, GL_RG32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::ivec3, GL_RGB32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::ivec4, GL_RGBA32I)
	
} } } // namespace eltecg::ogl::helper