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
	GLuint getInternalFormat(int num_of_channels = 3, int bytes_per_channel = 8);
	
	//Returns base format for a given internalFormat
	GLuint getBaseFormat(GLuint internalFormat);
	
	//Returns base type for a given internalFormat
	GLuint getBaseType(GLuint internalFormat);

	
/****************************************************************************
 *						Helper types										*/

	template<typename T> struct integral {}; //for integral formats

	//dummys: (ieee half float library is coming soon...)
	struct half {};
	struct half2 {};
	struct half3 {};
	struct half4 {};
	
/****************************************************************************
 *						Choose format										*/

template<typename T> struct _GetInternalFormat { 
	static constexpr GLenum GetFormat() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return 0; }
	static constexpr GLenum GetChannel() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return 0; }
	static constexpr GLenum GetBaseType() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return 0; }
};


#define DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(cpp_type, gl_type, gl_channel, gl_baseType) \
	template<> struct _GetInternalFormat<cpp_type> { \
		static constexpr GLenum GetFormat() { return gl_type; } \
		static constexpr GLenum GetChannel() { return gl_channel; }\
		static constexpr GLenum GetBaseType() { return gl_baseType; }	};

//[0,1] normalized formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint8_t, GL_R8, GL_RED, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec1, GL_R8, GL_RED, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec2, GL_RG8, GL_RG, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec3, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec4, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint16_t, GL_R16, GL_RED, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec1, GL_R16, GL_RED, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec2, GL_RG16, GL_RG, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec3, GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec4, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT)

//[-1,1] normalized formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(int8_t, GL_R8_SNORM, GL_RED, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec1, GL_R8_SNORM, GL_RED, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec2, GL_RG8_SNORM, GL_RG, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec3, GL_RGB8_SNORM, GL_RGB, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec4, GL_RGBA8_SNORM, GL_RGBA, GL_BYTE)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(int16_t, GL_R16_SNORM, GL_RED, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec1, GL_R16_SNORM, GL_RED, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec2, GL_RG16_SNORM, GL_RG, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec3, GL_RGB16_SNORM, GL_RGB, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec4, GL_RGBA16_SNORM, GL_RGBA, GL_SHORT)

// floating point formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half, GL_R16F, GL_RED, 0) // THIS IS WRONG, TODO: is it possible to upload half floats?
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half2, GL_RG16F, GL_RG, 0)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half3, GL_RGB16F, GL_RGB, 0)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half4, GL_RGBA16F, GL_RGBA, 0)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(float, GL_R32F, GL_RED, GL_FLOAT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec1, GL_R32F, GL_RED, GL_FLOAT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec2, GL_RG32F, GL_RG, GL_FLOAT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec3, GL_RGB32F, GL_RGB, GL_FLOAT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec4, GL_RGBA32F, GL_RGBA, GL_FLOAT)

//integer formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint8_t>, GL_R8UI, GL_RED, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec1>, GL_R8UI, GL_RED, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec2>, GL_RG8UI, GL_RG, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec3>, GL_RGB8UI, GL_RGB, GL_UNSIGNED_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec4>, GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_BYTE)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint16_t>, GL_R16UI, GL_RED, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec1>, GL_R16UI, GL_RED, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec2>, GL_RG16UI, GL_RG, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec3>, GL_RGB16UI, GL_RGB, GL_UNSIGNED_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec4>, GL_RGBA16UI, GL_RGBA, GL_UNSIGNED_SHORT)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint32_t>, GL_R32UI, GL_RED, GL_UNSIGNED_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec1>, GL_R32UI, GL_RED, GL_UNSIGNED_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec2>, GL_RG32UI, GL_RG, GL_UNSIGNED_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec3>, GL_RGB32UI, GL_RGB, GL_UNSIGNED_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec4>, GL_RGBA32UI, GL_RGBA, GL_UNSIGNED_INT)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int8_t>, GL_R8I, GL_RED, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec1>, GL_R8I, GL_RED, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec2>, GL_RG8I, GL_RG, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec3>, GL_RGB8I, GL_RGB, GL_BYTE)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec4>, GL_RGBA8I, GL_RGBA, GL_BYTE)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int16_t>, GL_R16I, GL_RED, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec1>, GL_R16I, GL_RED, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec2>, GL_RG16I, GL_RG, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec3>, GL_RGB16I, GL_RGB, GL_SHORT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec4>, GL_RGBA16I, GL_RGBA, GL_SHORT)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int32_t>, GL_R32I, GL_RED, GL_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec1>, GL_R32I, GL_RED, GL_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec2>, GL_RG32I, GL_RG, GL_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec3>, GL_RGB32I, GL_RGB, GL_INT)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec4>, GL_RGBA32I, GL_RGBA, GL_INT)
	
#undef DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION

template<typename T> constexpr GLenum getInternalFormat() { return _GetInternalFormat< std::remove_cv_t<std::remove_reference_t<T>>>::GetFormat(); }
template<typename T> constexpr GLenum getInternalChannel() { return _GetInternalFormat< std::remove_cv_t<std::remove_reference_t<T>>>::GetChannel(); }
template<typename T> constexpr GLenum getInternalBaseType() { return _GetInternalFormat< std::remove_cv_t<std::remove_reference_t<T>>>::GetBaseType(); }

} } } // namespace eltecg::ogl::helper