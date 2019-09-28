#pragma once
#include "object.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
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
 *						Helper types										*/

	template<typename T> struct integral {}; //for integral formats

	//dummys: (ieee half float library is coming soon...)
	struct half {};
	struct half2 {};
	struct half3 {};
	struct half4 {};
	
/****************************************************************************
 *						Choose format										*/

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
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half2, GL_RG16F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half3, GL_RGB16F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(half4, GL_RGBA16F)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(float, GL_R32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec1, GL_R32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec2, GL_RG32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec3, GL_RGB32F)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec4, GL_RGBA32F)

//integer formats

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint8_t>, GL_R8UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec1>, GL_R8UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec2>, GL_RG8UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec3>, GL_RGB8UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u8vec4>, GL_RGBA8UI)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint16_t>, GL_R16UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec1>, GL_R16UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec2>, GL_RG16UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec3>, GL_RGB16UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::u16vec4>, GL_RGBA16UI)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<uint32_t>, GL_R32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec1>, GL_R32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec2>, GL_RG32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec3>, GL_RGB32UI)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::uvec4>, GL_RGBA32UI)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int8_t>, GL_R8I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec1>, GL_R8I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec2>, GL_RG8I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec3>, GL_RGB8I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i8vec4>, GL_RGBA8I)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int16_t>, GL_R16I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec1>, GL_R16I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec2>, GL_RG16I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec3>, GL_RGB16I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::i16vec4>, GL_RGBA16I)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<int32_t>, GL_R32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec1>, GL_R32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec2>, GL_RG32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec3>, GL_RGB32I)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(integral<glm::ivec4>, GL_RGBA32I)
	
#undef DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION

template<typename T> constexpr GLenum getInternalFormat() { return _GetInternalFormat< std::remove_cv_t<std::remove_reference_t<T>>>::Get(); }

} } } // namespace eltecg::ogl::helper