#include "UniformTypes.hpp"

OpenGL_BaseType::ValueType OpenGL_BaseType::GetCppTypeFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:				return GLfloat();		break;
	case GL_FLOAT_VEC2:			return glm::vec2();		break;
	case GL_FLOAT_VEC3:			return glm::vec3();		break;
	case GL_FLOAT_VEC4:			return glm::vec4();		break;
	case GL_DOUBLE:				return GLdouble();		break;
	case GL_DOUBLE_VEC2:		return glm::dvec2();	break;
	case GL_DOUBLE_VEC3:		return glm::dvec3();	break;
	case GL_DOUBLE_VEC4:		return glm::dvec4();	break;

	case GL_INT:				return GLint();			break;
	case GL_INT_VEC2:			return glm::ivec2();	break;
	case GL_INT_VEC3:			return glm::ivec3();	break;
	case GL_INT_VEC4:			return glm::ivec4();	break;
	case GL_UNSIGNED_INT:		return GLuint();		break;
	case GL_UNSIGNED_INT_VEC2:	return glm::uvec2();	break;
	case GL_UNSIGNED_INT_VEC3:	return glm::uvec3();	break;
	case GL_UNSIGNED_INT_VEC4:	return glm::uvec4();	break;

	case GL_FLOAT_MAT2:			return glm::mat2();		break;
	case GL_FLOAT_MAT3:			return glm::mat3();		break;
	case GL_FLOAT_MAT4:			return glm::mat4();		break;
	case GL_FLOAT_MAT2x3:		return glm::mat2x3();	break;
	case GL_FLOAT_MAT3x2:		return glm::mat3x2();	break;
	case GL_FLOAT_MAT2x4:		return glm::mat2x4();	break;
	case GL_FLOAT_MAT4x2:		return glm::mat4x2();	break;
	case GL_FLOAT_MAT3x4:		return glm::mat3x4();	break;
	case GL_FLOAT_MAT4x3:		return glm::mat4x3();	break;

	case GL_DOUBLE_MAT2:		return glm::dmat2();	break;
	case GL_DOUBLE_MAT3:		return glm::dmat3();	break;
	case GL_DOUBLE_MAT4:		return glm::dmat4();	break;
	case GL_DOUBLE_MAT2x3:		return glm::dmat2x3();	break;
	case GL_DOUBLE_MAT3x2:		return glm::dmat3x2();	break;
	case GL_DOUBLE_MAT2x4:		return glm::dmat2x4();	break;
	case GL_DOUBLE_MAT4x2:		return glm::dmat4x2();	break;
	case GL_DOUBLE_MAT3x4:		return glm::dmat3x4();	break;
	case GL_DOUBLE_MAT4x3:		return glm::dmat4x3();	break;

	default: return UniformLowLevelBase::uni_hash_type(0); break;
	}
}

std::string OpenGL_BaseType::GetTypeNameFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:				return "float";		break;
	case GL_FLOAT_VEC2:			return "vec2";		break;
	case GL_FLOAT_VEC3:			return "vec3";		break;
	case GL_FLOAT_VEC4:			return "vec4";		break;
	case GL_DOUBLE:				return "double";	break;
	case GL_DOUBLE_VEC2:		return "dvec2";		break;
	case GL_DOUBLE_VEC3:		return "dvec3";		break;
	case GL_DOUBLE_VEC4:		return "dvec4";		break;
		
	case GL_INT:				return "int";		break;
	case GL_INT_VEC2:			return "ivec2";		break;
	case GL_INT_VEC3:			return "ivec3";		break;
	case GL_INT_VEC4:			return "ivec4";		break;
	case GL_UNSIGNED_INT:		return "uint";		break;
	case GL_UNSIGNED_INT_VEC2:	return "uvec2";		break;
	case GL_UNSIGNED_INT_VEC3:	return "uvec3";		break;
	case GL_UNSIGNED_INT_VEC4:	return "uvec4";		break;
	
	case GL_FLOAT_MAT2:			return "mat2";		break;
	case GL_FLOAT_MAT3:			return "mat3";		break;
	case GL_FLOAT_MAT4:			return "mat4";		break;
	case GL_FLOAT_MAT2x3:		return "mat2x3";	break;
	case GL_FLOAT_MAT3x2:		return "mat3x2";	break;
	case GL_FLOAT_MAT2x4:		return "mat2x4";	break;
	case GL_FLOAT_MAT4x2:		return "mat4x2";	break;
	case GL_FLOAT_MAT3x4:		return "mat3x4";	break;
	case GL_FLOAT_MAT4x3:		return "mat4x3";	break;
	
	case GL_DOUBLE_MAT2:		return "dmat2";		break;
	case GL_DOUBLE_MAT3:		return "dmat3";		break;
	case GL_DOUBLE_MAT4:		return "dmat4";		break;
	case GL_DOUBLE_MAT2x3:		return "dmat2x3";	break;
	case GL_DOUBLE_MAT3x2:		return "dmat3x2";	break;
	case GL_DOUBLE_MAT2x4:		return "dmat2x4";	break;
	case GL_DOUBLE_MAT4x2:		return "dmat4x2";	break;
	case GL_DOUBLE_MAT3x4:		return "dmat3x4";	break;
	case GL_DOUBLE_MAT4x3:		return "dmat4x3";	break;

	case GL_SAMPLER_1D:			return "sampler1D";	break;
	case GL_SAMPLER_2D:			return "sampler2D";	break;
	case GL_SAMPLER_3D:			return "sampler1D";	break;
	case GL_SAMPLER_CUBE:		return "samplerCube";	break;

	default: return "unknown type"; break;
	}
}
