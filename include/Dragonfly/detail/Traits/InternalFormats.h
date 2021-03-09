#pragma once
#include "../object.h"
#include <glm/glm.hpp>
#include <cstdint>

namespace df {
	
template<typename T> struct integral { T t; }; //for integral formats

//dummys: (ieee half float library is coming soon...)
struct half  { uint16_t dummy[1]; };
struct half2 { uint16_t dummy[2]; };
struct half3 { uint16_t dummy[3]; };
struct half4 { uint16_t dummy[4]; };

struct r11g11b10 { uint32_t dummy; };

//only 3 options and 24bit type doesnt exist anyway
struct depth16 { uint8_t dummy[2]; };
struct depth24 { uint8_t dummy[3]; };
struct depth32F { float dummy; };
	
struct stencil1 {};
struct stencil4 {};
struct stencil8 { uint8_t dummy; };
struct stencil16 { uint16_t dummy; };

struct depth24stencil8 { uint8_t dummy[3]; uint8_t dummy2; };
struct depth32Fstencil8 { float dummy; uint8_t dummy2; };
	
namespace detail {

enum TEXEL_TYPE {
	NONE = 0,
	COLOR = 1,
	DEPTH = 2,
	STENCIL = 4,
	DEPTH_STENCIL = 6,
};

template<typename T> struct TypeInfo {
	static_assert(sizeof(T)<=0, "Invalid template argument type");
	static constexpr GLenum glTexInternalFormat;
	static constexpr GLenum glTexChannelFormat;
	static constexpr GLenum glTexBaseType;
	static constexpr uint32_t bits;
	static constexpr uint32_t bytes;
	static constexpr uint32_t channels;
	static constexpr TEXEL_TYPE texelType;
};

#define DEF_TYPEINFO_ALL(CPP_TYPE, BASE_TYPE, GL_TEX_INTERNAL_FORMAT, GL_TEX_CHANNEL_FORMAT, GL_TEX_BASE_TYPE, CHANNELS, BYTES, BITS) \
	template<> struct TypeInfo<CPP_TYPE> {										\
		using BaseType								= BASE_TYPE;				\
		static constexpr GLenum glTexInternalFormat = GL_TEX_INTERNAL_FORMAT;	\
		static constexpr GLenum glTexChannelFormat	= GL_TEX_CHANNEL_FORMAT;	\
		static constexpr GLenum glTexBaseType		= GL_TEX_BASE_TYPE;			\
		static constexpr uint32_t channels			= CHANNELS;					\
		static constexpr uint32_t bytes				= BYTES;					\
		static constexpr uint32_t bits				= BITS;						\
};

#define DEF_TYPEINFO(CPP_TYPE, BASE_TYPE, GL_TEX_INTERNAL_FORMAT, GL_TEX_CHANNEL_FORMAT, GL_TEX_BASE_TYPE) \
	DEF_TYPEINFO_ALL(CPP_TYPE, BASE_TYPE, GL_TEX_INTERNAL_FORMAT, GL_TEX_CHANNEL_FORMAT, GL_TEX_BASE_TYPE, sizeof(CPP_TYPE)/sizeof(BASE_TYPE), sizeof(CPP_TYPE), 8*sizeof(CPP_TYPE))


//template<typename T> struct _GetInternalFormat { 
//	static constexpr GLenum GetFormat()   { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return GL_INVALID_ENUM; }
//	static constexpr GLenum GetChannel()  { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return GL_INVALID_ENUM; }
//	static constexpr GLenum GetBaseType() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return GL_INVALID_ENUM; }
//};
template <typename T> struct _is_integral_format : public std::false_type {};
template <typename InternalFormat_> struct _is_integral_format<integral< InternalFormat_>> : public std::true_type{};

/***** Format functions *****/

//Returns the OpenGL internal format for the corresponding C++ type (T)
template<typename T> constexpr GLenum getInternalFormat() { return TypeInfo<std::remove_cv_t<std::remove_reference_t<T>>>::glTexInternalFormat; }

//Returns the number of channels for template parameter T
template<typename T> constexpr GLenum getInternalChannel() { return TypeInfo<std::remove_cv_t<std::remove_reference_t<T>>>::glTexChannelFormat; }

//Returns OpenGL base type for template parameter T
template<typename T> constexpr GLenum getInternalBaseType() { return TypeInfo< std::remove_cv_t<std::remove_reference_t<T>>>::glTexBaseFormat; }

//Returns the most fitting internalFormat for the number of cannels and bytes per channel. Whenever available, the floating point version is returned
GLuint getInternalFormat(int num_of_channels = 3, int bytes_per_channel = 8);

//Returns base format for a given internalFormat
GLuint getBaseFormat(GLuint internalFormat);

//Returns base type for a given internalFormat
GLuint getBaseType(GLuint internalFormat);

//Retuns if the type is made with integral<>. TODO redesign to have unsigend and other base types
template<typename T> constexpr bool isInternalFormatIntegralType() { return _is_integral_format<T>::value; }

/***** Type definitions *****/

//[0,1] normalized formats

DEF_TYPEINFO(uint8_t,     uint8_t, GL_R8,	 GL_RED,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(glm::u8vec1, uint8_t, GL_R8,	 GL_RED,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(glm::u8vec2, uint8_t, GL_RG8,	 GL_RG,   GL_UNSIGNED_BYTE)
DEF_TYPEINFO(glm::u8vec3, uint8_t, GL_RGB8,  GL_RGB,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(glm::u8vec4, uint8_t, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE)

DEF_TYPEINFO(uint16_t,     uint16_t, GL_R16,    GL_RED,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(glm::u16vec1, uint16_t, GL_R16,    GL_RED,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(glm::u16vec2, uint16_t, GL_RG16,   GL_RG,   GL_UNSIGNED_SHORT)
DEF_TYPEINFO(glm::u16vec3, uint16_t, GL_RGB16,  GL_RGB,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(glm::u16vec4, uint16_t, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT)

//[-1,1] normalized formats

DEF_TYPEINFO(int8_t,	  int8_t, GL_R8_SNORM,	  GL_RED,  GL_BYTE)
DEF_TYPEINFO(glm::i8vec1, int8_t, GL_R8_SNORM,    GL_RED,  GL_BYTE)
DEF_TYPEINFO(glm::i8vec2, int8_t, GL_RG8_SNORM,   GL_RG,   GL_BYTE)
DEF_TYPEINFO(glm::i8vec3, int8_t, GL_RGB8_SNORM,  GL_RGB,  GL_BYTE)
DEF_TYPEINFO(glm::i8vec4, int8_t, GL_RGBA8_SNORM, GL_RGBA, GL_BYTE)

DEF_TYPEINFO(int16_t,      int16_t, GL_R16_SNORM,    GL_RED,  GL_SHORT)
DEF_TYPEINFO(glm::i16vec1, int16_t, GL_R16_SNORM,    GL_RED,  GL_SHORT)
DEF_TYPEINFO(glm::i16vec2, int16_t, GL_RG16_SNORM,   GL_RG,   GL_SHORT)
DEF_TYPEINFO(glm::i16vec3, int16_t, GL_RGB16_SNORM,  GL_RGB,  GL_SHORT)
DEF_TYPEINFO(glm::i16vec4, int16_t, GL_RGBA16_SNORM, GL_RGBA, GL_SHORT)

// floating point formats

DEF_TYPEINFO(half , half, GL_R16F,    GL_RED,  GL_HALF_FLOAT)
DEF_TYPEINFO(half2, half, GL_RG16F,   GL_RG,   GL_HALF_FLOAT)
DEF_TYPEINFO(half3, half, GL_RGB16F,  GL_RGB,  GL_HALF_FLOAT)
DEF_TYPEINFO(half4, half, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT)

DEF_TYPEINFO_ALL(r11g11b10, uint32_t, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 3, 4, 32)

DEF_TYPEINFO(float,     float, GL_R32F,    GL_RED,  GL_FLOAT)
DEF_TYPEINFO(glm::vec1, float, GL_R32F,    GL_RED,  GL_FLOAT)
DEF_TYPEINFO(glm::vec2, float, GL_RG32F,   GL_RG,   GL_FLOAT)
DEF_TYPEINFO(glm::vec3, float, GL_RGB32F,  GL_RGB,  GL_FLOAT)
DEF_TYPEINFO(glm::vec4, float, GL_RGBA32F, GL_RGBA, GL_FLOAT)

//integer formats

DEF_TYPEINFO(integral<uint8_t>,     uint8_t, GL_R8UI,    GL_RED_INTEGER,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(integral<glm::u8vec1>, uint8_t, GL_R8UI,    GL_RED_INTEGER,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(integral<glm::u8vec2>, uint8_t, GL_RG8UI,   GL_RG_INTEGER,   GL_UNSIGNED_BYTE)
DEF_TYPEINFO(integral<glm::u8vec3>, uint8_t, GL_RGB8UI,  GL_RGB_INTEGER,  GL_UNSIGNED_BYTE)
DEF_TYPEINFO(integral<glm::u8vec4>, uint8_t, GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE)

DEF_TYPEINFO(integral<uint16_t>,     uint16_t, GL_R16UI,    GL_RED_INTEGER,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(integral<glm::u16vec1>, uint16_t, GL_R16UI,    GL_RED_INTEGER,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(integral<glm::u16vec2>, uint16_t, GL_RG16UI,   GL_RG_INTEGER,   GL_UNSIGNED_SHORT)
DEF_TYPEINFO(integral<glm::u16vec3>, uint16_t, GL_RGB16UI,  GL_RGB_INTEGER,  GL_UNSIGNED_SHORT)
DEF_TYPEINFO(integral<glm::u16vec4>, uint16_t, GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT)

DEF_TYPEINFO(integral<uint32_t>,   uint32_t, GL_R32UI,    GL_RED_INTEGER,  GL_UNSIGNED_INT)
DEF_TYPEINFO(integral<glm::uvec1>, uint32_t, GL_R32UI,    GL_RED_INTEGER,  GL_UNSIGNED_INT)
DEF_TYPEINFO(integral<glm::uvec2>, uint32_t, GL_RG32UI,   GL_RG_INTEGER,   GL_UNSIGNED_INT)
DEF_TYPEINFO(integral<glm::uvec3>, uint32_t, GL_RGB32UI,  GL_RGB_INTEGER,  GL_UNSIGNED_INT)
DEF_TYPEINFO(integral<glm::uvec4>, uint32_t, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT)

DEF_TYPEINFO(integral<int8_t>,	    int8_t, GL_R8I,    GL_RED_INTEGER,  GL_BYTE)
DEF_TYPEINFO(integral<glm::i8vec1>, int8_t, GL_R8I,    GL_RED_INTEGER,  GL_BYTE)
DEF_TYPEINFO(integral<glm::i8vec2>, int8_t, GL_RG8I,   GL_RG_INTEGER,   GL_BYTE)
DEF_TYPEINFO(integral<glm::i8vec3>, int8_t, GL_RGB8I,  GL_RGB_INTEGER,  GL_BYTE)
DEF_TYPEINFO(integral<glm::i8vec4>, int8_t, GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE)

DEF_TYPEINFO(integral<int16_t>,      int16_t, GL_R16I,    GL_RED_INTEGER,  GL_SHORT)
DEF_TYPEINFO(integral<glm::i16vec1>, int16_t, GL_R16I,    GL_RED_INTEGER,  GL_SHORT)
DEF_TYPEINFO(integral<glm::i16vec2>, int16_t, GL_RG16I,   GL_RG_INTEGER,   GL_SHORT)
DEF_TYPEINFO(integral<glm::i16vec3>, int16_t, GL_RGB16I,  GL_RGB_INTEGER,  GL_SHORT)
DEF_TYPEINFO(integral<glm::i16vec4>, int16_t, GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT)

DEF_TYPEINFO(integral<int32_t>,    int32_t, GL_R32I,    GL_RED_INTEGER,  GL_INT)
DEF_TYPEINFO(integral<glm::ivec1>, int32_t, GL_R32I,    GL_RED_INTEGER,  GL_INT)
DEF_TYPEINFO(integral<glm::ivec2>, int32_t, GL_RG32I,   GL_RG_INTEGER,   GL_INT)
DEF_TYPEINFO(integral<glm::ivec3>, int32_t, GL_RGB32I,  GL_RGB_INTEGER,  GL_INT)
DEF_TYPEINFO(integral<glm::ivec4>, int32_t, GL_RGBA32I, GL_RGBA_INTEGER, GL_INT)

// DEPTH and STENCIL

DEF_TYPEINFO(depth16,  uint16_t, GL_DEPTH_COMPONENT16,  GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT)
DEF_TYPEINFO(depth24,  depth24,  GL_DEPTH_COMPONENT24,  GL_DEPTH_COMPONENT, 0)
DEF_TYPEINFO(depth32F, float,    GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT)

DEF_TYPEINFO_ALL(stencil1, stencil1, GL_STENCIL_INDEX1,  GL_STENCIL_INDEX, 0, 1, 1, 1)
DEF_TYPEINFO_ALL(stencil4, stencil1, GL_STENCIL_INDEX4,  GL_STENCIL_INDEX, 0, 1, 1, 4)
DEF_TYPEINFO(stencil8,     uint8_t,	 GL_STENCIL_INDEX8,  GL_STENCIL_INDEX, GL_UNSIGNED_BYTE)
DEF_TYPEINFO(stencil16,    uint16_t, GL_STENCIL_INDEX16, GL_STENCIL_INDEX, GL_UNSIGNED_SHORT)

DEF_TYPEINFO_ALL(depth24stencil8, depth24stencil8,  GL_DEPTH24_STENCIL8,  GL_DEPTH_STENCIL, 0, 2, 4, 32)
DEF_TYPEINFO_ALL(depth32Fstencil8, depth24stencil8, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, 0, 2, 5, 40)


//#undef DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION

#undef DEF_TYPEINFO
#undef DEF_TYPEINFO_ALL

} } // namespace df::detail