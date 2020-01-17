#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <variant>
#include <string>
#include "../../config.h"

namespace df
{

class UniformLowLevelBase
{
	//type magic
public:
	struct uni_hash_type { size_t h; uni_hash_type(size_t h) : h(h) {} };
	template<class... Types > struct type_list {};
	using valid_types = UniformLowLevelBase::type_list<
		GLfloat,	glm::vec2,	glm::vec3,	glm::vec4,
		GLdouble,	glm::dvec2, glm::dvec3, glm::dvec4,
		GLint,		glm::ivec2, glm::ivec3, glm::ivec4,
		GLuint,		glm::uvec2, glm::uvec3, glm::uvec4,
		glm::mat2,	glm::mat3,	glm::mat4,	glm::mat2x3,	glm::mat3x2,	glm::mat2x4,	glm::mat4x2,	glm::mat3x4,	glm::mat4x3,
		glm::dmat2, glm::dmat3, glm::dmat4, glm::dmat2x3,	glm::dmat3x2,	glm::dmat2x4,	glm::dmat4x2,	glm::dmat3x4,	glm::dmat4x3
	>;
	template<class... Types> static std::variant<UniformLowLevelBase::uni_hash_type, Types...> get_types_as_variant(UniformLowLevelBase::type_list<Types...>) {
		static_assert(false, "This function should never (ever) be called.");
	}
protected:
	template<typename T, typename LIST_T>	struct isListMember { /*static_assert(false, "Only use this on type_list.");*/ };

	template<typename T, typename... ALL_T>
	struct isListMember<T, UniformLowLevelBase::type_list<ALL_T...>> : public std::disjunction<std::is_same<T, ALL_T>...> {};

	template<typename T, typename... ALL_T>
	static constexpr bool is_list_member() { return UniformLowLevelBase::isListMember<std::remove_cv_t<std::remove_reference_t<T>>, ALL_T...>::value; }

protected:
	template<typename ValType>
	static void SetUni(GLuint loc, const ValType& val) { static_assert(false, "Cannot process this uniform type (or this value type is yet to be implemented)."); }

	template<> static inline void SetUni(GLuint loc, const uni_hash_type& val) { ASSERT(false, "This should never actually be called."); }

	template<> static inline void SetUni(GLuint loc, const GLfloat& val) { glUniform1f(loc, val); }
	template<> static inline void SetUni(GLuint loc, const glm::vec2& val) { glUniform2fv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::vec3& val) { glUniform3fv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::vec4& val) { glUniform4fv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const GLdouble& val) { glUniform1d(loc, val); }
	template<> static inline void SetUni(GLuint loc, const glm::dvec2& val) { glUniform2dv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::dvec3& val) { glUniform3dv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::dvec4& val) { glUniform4dv(loc, 1, &val.x); }

	template<> static inline void SetUni(GLuint loc, const GLint& val) { glUniform1i(loc, val); }
	template<> static inline void SetUni(GLuint loc, const glm::ivec2& val) { glUniform2iv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::ivec3& val) { glUniform3iv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::ivec4& val) { glUniform4iv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const GLuint& val) { glUniform1ui(loc, val); }
	template<> static inline void SetUni(GLuint loc, const glm::uvec2& val) { glUniform2uiv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::uvec3& val) { glUniform3uiv(loc, 1, &val.x); }
	template<> static inline void SetUni(GLuint loc, const glm::uvec4& val) { glUniform4uiv(loc, 1, &val.x); }

	template<> static inline void SetUni(GLuint loc, const glm::mat2& val) { glUniformMatrix2fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat3& val) { glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat4& val) { glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat2x3& val) { glUniformMatrix2x3fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat3x2& val) { glUniformMatrix3x2fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat2x4& val) { glUniformMatrix2x4fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat4x2& val) { glUniformMatrix4x2fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat3x4& val) { glUniformMatrix3x4fv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::mat4x3& val) { glUniformMatrix4x3fv(loc, 1, GL_FALSE, &val[0][0]); }

	template<> static inline void SetUni(GLuint loc, const glm::dmat2& val) { glUniformMatrix2dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat3& val) { glUniformMatrix3dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat4& val) { glUniformMatrix4dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat2x3& val) { glUniformMatrix2x3dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat3x2& val) { glUniformMatrix3x2dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat2x4& val) { glUniformMatrix2x4dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat4x2& val) { glUniformMatrix4x2dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat3x4& val) { glUniformMatrix3x4dv(loc, 1, GL_FALSE, &val[0][0]); }
	template<> static inline void SetUni(GLuint loc, const glm::dmat4x3& val) { glUniformMatrix4x3dv(loc, 1, GL_FALSE, &val[0][0]); }

protected:
	UniformLowLevelBase() {}
public:
	inline void Render() {} //overwrite this
};

template<typename T> struct _GetOpenGLType { static constexpr GLenum Get() { static_assert(false, "Cannot process this uniform type (or this type is yet to be implemented)."); return 0; } };

#define DEF_CPP2OGL_TYPE(cpp_type, gl_type) template<> struct _GetOpenGLType<cpp_type> { static constexpr GLenum Get() { return gl_type; } };

DEF_CPP2OGL_TYPE(GLfloat, GL_FLOAT)
DEF_CPP2OGL_TYPE(glm::vec2, GL_FLOAT_VEC2)
DEF_CPP2OGL_TYPE(glm::vec3, GL_FLOAT_VEC3)
DEF_CPP2OGL_TYPE(glm::vec4, GL_FLOAT_VEC4)
DEF_CPP2OGL_TYPE(GLdouble, GL_DOUBLE)
DEF_CPP2OGL_TYPE(glm::dvec2, GL_DOUBLE_VEC2)
DEF_CPP2OGL_TYPE(glm::dvec3, GL_DOUBLE_VEC3)
DEF_CPP2OGL_TYPE(glm::dvec4, GL_DOUBLE_VEC4)

DEF_CPP2OGL_TYPE(GLint, GL_INT)
DEF_CPP2OGL_TYPE(glm::ivec2, GL_INT_VEC2)
DEF_CPP2OGL_TYPE(glm::ivec3, GL_INT_VEC3)
DEF_CPP2OGL_TYPE(glm::ivec4, GL_INT_VEC4)
DEF_CPP2OGL_TYPE(GLuint, GL_UNSIGNED_INT)
DEF_CPP2OGL_TYPE(glm::uvec2, GL_UNSIGNED_INT_VEC2)
DEF_CPP2OGL_TYPE(glm::uvec3, GL_UNSIGNED_INT_VEC3)
DEF_CPP2OGL_TYPE(glm::uvec4, GL_UNSIGNED_INT_VEC4)

DEF_CPP2OGL_TYPE(glm::mat2, GL_FLOAT_MAT2)
DEF_CPP2OGL_TYPE(glm::mat3, GL_FLOAT_MAT3)
DEF_CPP2OGL_TYPE(glm::mat4, GL_FLOAT_MAT4)
DEF_CPP2OGL_TYPE(glm::mat2x3, GL_FLOAT_MAT2x3)
DEF_CPP2OGL_TYPE(glm::mat3x2, GL_FLOAT_MAT3x2)
DEF_CPP2OGL_TYPE(glm::mat2x4, GL_FLOAT_MAT2x4)
DEF_CPP2OGL_TYPE(glm::mat4x2, GL_FLOAT_MAT4x2)
DEF_CPP2OGL_TYPE(glm::mat3x4, GL_FLOAT_MAT3x4)
DEF_CPP2OGL_TYPE(glm::mat4x3, GL_FLOAT_MAT4x3)

DEF_CPP2OGL_TYPE(glm::dmat2, GL_DOUBLE_MAT2)
DEF_CPP2OGL_TYPE(glm::dmat3, GL_DOUBLE_MAT3)
DEF_CPP2OGL_TYPE(glm::dmat4, GL_DOUBLE_MAT4)
DEF_CPP2OGL_TYPE(glm::dmat2x3, GL_DOUBLE_MAT2x3)
DEF_CPP2OGL_TYPE(glm::dmat3x2, GL_DOUBLE_MAT3x2)
DEF_CPP2OGL_TYPE(glm::dmat2x4, GL_DOUBLE_MAT2x4)
DEF_CPP2OGL_TYPE(glm::dmat4x2, GL_DOUBLE_MAT4x2)
DEF_CPP2OGL_TYPE(glm::dmat3x4, GL_DOUBLE_MAT3x4)
DEF_CPP2OGL_TYPE(glm::dmat4x3, GL_DOUBLE_MAT4x3)

//not uniforms:
DEF_CPP2OGL_TYPE(GLchar, GL_BYTE)
DEF_CPP2OGL_TYPE(GLubyte, GL_UNSIGNED_BYTE)
DEF_CPP2OGL_TYPE(GLshort, GL_SHORT)
DEF_CPP2OGL_TYPE(GLushort, GL_UNSIGNED_SHORT)

#undef DEF_CPP2OGL_TYPE

template<typename T> constexpr GLenum getOpenGLType() { return _GetOpenGLType< std::remove_cv_t<std::remove_reference_t<T>>>::Get(); }

bool isOpenGLTextureType(GLenum type);

//Useful for converting an opengl type to a variant
class OpenGL_BaseType
{
	using ValueType = decltype(UniformLowLevelBase::get_types_as_variant(UniformLowLevelBase::valid_types{}));
	static ValueType GetCppTypeFromOpenGlType(GLenum type);		//in UniformTypes.cpp
	OpenGL_BaseType() = delete;		//variant's default constructor is deleted anyway
protected:
	static std::string GetTypeNameFromOpenGlType(GLenum type);
	OpenGL_BaseType(GLenum type) : variant(GetCppTypeFromOpenGlType(type)){}
	ValueType variant;
};

} //namespace df