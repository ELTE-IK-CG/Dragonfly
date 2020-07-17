#pragma once

#include <GL/glew.h>
#include <glm/fwd.hpp>

template<typename T> struct _GetInternalFormat {
	static constexpr GLenum GetComponentCount() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return GL_INVALID_ENUM; }
	static constexpr GLenum GetBaseType() { static_assert(false, "Cannot process this internal format type (or this type is yet to be implemented)."); return GL_INVALID_ENUM; }
};

#define DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(cpp_type, gl_compCount, gl_baseType, is_integral)		\
	template<> struct _GetInternalFormat<cpp_type> {											\
		static constexpr short GetComponentCount() { return gl_compCount; }					\
		static constexpr GLenum GetBaseType() { return gl_baseType; }							\
		static constexpr bool IsIntegral() { return is_integral; }};


// DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(uint8_t, GL_R8, GL_RED, GL_UNSIGNED_BYTE)
// DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec1, GL_R8, GL_RED, GL_UNSIGNED_BYTE)

// Unsigned Integers
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uint8_t, 1, GL_UNSIGNED_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec1, 1, GL_UNSIGNED_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec2, 2, GL_UNSIGNED_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec3, 3, GL_UNSIGNED_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u8vec4, 4, GL_UNSIGNED_BYTE, true)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uint16_t, 1, GL_UNSIGNED_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec1, 1, GL_UNSIGNED_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec2, 2, GL_UNSIGNED_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec3, 3, GL_UNSIGNED_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u16vec4, 4, GL_UNSIGNED_SHORT, true)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::uint32_t, 1, GL_UNSIGNED_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u32vec1, 1, GL_UNSIGNED_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u32vec2, 2, GL_UNSIGNED_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u32vec3, 3, GL_UNSIGNED_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::u32vec4, 4, GL_UNSIGNED_INT, true)

// Signed Integers
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::int8_t, 1, GL_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec1, 1, GL_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec2, 2, GL_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec3, 3, GL_BYTE, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i8vec4, 4, GL_BYTE, true)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::int16_t, 1, GL_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec1, 1, GL_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec2, 2, GL_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec3, 3, GL_SHORT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i16vec4, 4, GL_SHORT, true)

DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::int32_t, 1, GL_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i32vec1, 1, GL_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i32vec2, 2, GL_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i32vec3, 3, GL_INT, true)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::i32vec4, 4, GL_INT, true)

// Floats (32-bit)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::float32_t, 1, GL_FLOAT, false)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec1, 1, GL_FLOAT, false)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec2, 2, GL_FLOAT, false)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec3, 3, GL_FLOAT, false)
DEFINE_CPP_TO_INERNAL_FORMAT_CONVERSION(glm::vec4, 4, GL_FLOAT, false)


// void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
// void glVertexAttribIPointer(GLuint index, GLint size, GLenum type,                       GLsizei stride, const void* pointer);