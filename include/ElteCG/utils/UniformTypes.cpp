#include "UniformTypes.hpp"

OpenGL_BaseType::ValueType OpenGL_BaseType::GetCppTypeFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:										return GLfloat();				break;
	case GL_FLOAT_VEC2:									return glm::vec2();				break;
	case GL_FLOAT_VEC3:									return glm::vec3();				break;
	case GL_FLOAT_VEC4:									return glm::vec4();				break;
	case GL_DOUBLE:										return GLdouble();				break;
	case GL_DOUBLE_VEC2:								return glm::dvec2();			break;
	case GL_DOUBLE_VEC3:								return glm::dvec3();			break;
	case GL_DOUBLE_VEC4:								return glm::dvec4();			break;

	case GL_INT:										return GLint();					break;
	case GL_INT_VEC2:									return glm::ivec2();			break;
	case GL_INT_VEC3:									return glm::ivec3();			break;
	case GL_INT_VEC4:									return glm::ivec4();			break;
	case GL_UNSIGNED_INT:								return GLuint();				break;
	case GL_UNSIGNED_INT_VEC2:							return glm::uvec2();			break;
	case GL_UNSIGNED_INT_VEC3:							return glm::uvec3();			break;
	case GL_UNSIGNED_INT_VEC4:							return glm::uvec4();			break;

	case GL_FLOAT_MAT2:									return glm::mat2();				break;
	case GL_FLOAT_MAT3:									return glm::mat3();				break;
	case GL_FLOAT_MAT4:									return glm::mat4();				break;
	case GL_FLOAT_MAT2x3:								return glm::mat2x3();			break;
	case GL_FLOAT_MAT3x2:								return glm::mat3x2();			break;
	case GL_FLOAT_MAT2x4:								return glm::mat2x4();			break;
	case GL_FLOAT_MAT4x2:								return glm::mat4x2();			break;
	case GL_FLOAT_MAT3x4:								return glm::mat3x4();			break;
	case GL_FLOAT_MAT4x3:								return glm::mat4x3();			break;

	case GL_DOUBLE_MAT2:								return glm::dmat2();			break;
	case GL_DOUBLE_MAT3:								return glm::dmat3();			break;
	case GL_DOUBLE_MAT4:								return glm::dmat4();			break;
	case GL_DOUBLE_MAT2x3:								return glm::dmat2x3();			break;
	case GL_DOUBLE_MAT3x2:								return glm::dmat3x2();			break;
	case GL_DOUBLE_MAT2x4:								return glm::dmat2x4();			break;
	case GL_DOUBLE_MAT4x2:								return glm::dmat4x2();			break;
	case GL_DOUBLE_MAT3x4:								return glm::dmat3x4();			break;
	case GL_DOUBLE_MAT4x3:								return glm::dmat4x3();			break;

	default: return UniformLowLevelBase::uni_hash_type(0); break;
	}
}

std::string OpenGL_BaseType::GetTypeNameFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:										return "float";					break;
	case GL_FLOAT_VEC2:									return "vec2";					break;
	case GL_FLOAT_VEC3:									return "vec3";					break;
	case GL_FLOAT_VEC4:									return "vec4";					break;
	case GL_INT:										return "int";					break;
	case GL_INT_VEC2:									return "ivec2";					break;
	case GL_INT_VEC3:									return "ivec3";					break;
	case GL_INT_VEC4:									return "ivec4";					break;
	case GL_UNSIGNED_INT:								return "uint";					break;
	case GL_UNSIGNED_INT_VEC2:							return "uvec2";					break;
	case GL_UNSIGNED_INT_VEC3:							return "uvec3";					break;
	case GL_UNSIGNED_INT_VEC4:							return "uvec4";					break;
	
	case GL_FLOAT_MAT2:									return "mat2";					break;
	case GL_FLOAT_MAT3:									return "mat3";					break;
	case GL_FLOAT_MAT4:									return "mat4";					break;
	case GL_FLOAT_MAT2x3:								return "mat2x3";				break;
	case GL_FLOAT_MAT3x2:								return "mat3x2";				break;
	case GL_FLOAT_MAT2x4:								return "mat2x4";				break;
	case GL_FLOAT_MAT4x2:								return "mat4x2";				break;
	case GL_FLOAT_MAT3x4:								return "mat3x4";				break;
	case GL_FLOAT_MAT4x3:								return "mat4x3";				break;

#if OPENGL_VERSION >=41	
	case GL_DOUBLE:										return "double";				break;
	case GL_DOUBLE_VEC2:								return "dvec2";					break;
	case GL_DOUBLE_VEC3:								return "dvec3";					break;
	case GL_DOUBLE_VEC4:								return "dvec4";					break;

	case GL_DOUBLE_MAT2:								return "dmat2";					break;
	case GL_DOUBLE_MAT3:								return "dmat3";					break;
	case GL_DOUBLE_MAT4:								return "dmat4";					break;
	case GL_DOUBLE_MAT2x3:								return "dmat2x3";				break;
	case GL_DOUBLE_MAT3x2:								return "dmat3x2";				break;
	case GL_DOUBLE_MAT2x4:								return "dmat2x4";				break;
	case GL_DOUBLE_MAT4x2:								return "dmat4x2";				break;
	case GL_DOUBLE_MAT3x4:								return "dmat3x4";				break;
	case GL_DOUBLE_MAT4x3:								return "dmat4x3";				break;
#endif
	case GL_SAMPLER_1D:									return "sampler1D";				break;
	case GL_SAMPLER_2D:									return "sampler2D";				break;
	case GL_SAMPLER_3D:									return "sampler1D";				break;
	case GL_SAMPLER_CUBE:								return "samplerCube";			break;
	case GL_SAMPLER_1D_SHADOW:							return "sampler1DShadow";		break;
	case GL_SAMPLER_2D_SHADOW:							return "sampler2DShadow";		break;
	case GL_SAMPLER_1D_ARRAY:							return "sampler1DArray";		break;
	case GL_SAMPLER_2D_ARRAY:							return "sampler2DArray";		break;
	case GL_SAMPLER_1D_ARRAY_SHADOW:					return "sampler1DArrayShadow";	break;
	case GL_SAMPLER_2D_ARRAY_SHADOW:					return "sampler2DArrayShadow";	break;
	case GL_SAMPLER_2D_MULTISAMPLE:						return "sampler2DMS";			break;
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:				return "sampler2DMSArray";		break;
	case GL_SAMPLER_CUBE_SHADOW:						return "samplerCubeShadow";		break;
	case GL_SAMPLER_BUFFER:								return "samplerBuffer";			break;
	case GL_SAMPLER_2D_RECT:							return "sampler2DRect";			break;
	case GL_SAMPLER_2D_RECT_SHADOW:						return "sampler2DRectShadow";	break;

	case GL_INT_SAMPLER_1D:								return "isampler1D";			break;
	case GL_INT_SAMPLER_2D:								return "isampler2D";			break;
	case GL_INT_SAMPLER_3D:								return "isampler1D";			break;
	case GL_INT_SAMPLER_CUBE:							return "isamplerCube";			break;
	case GL_INT_SAMPLER_1D_ARRAY:						return "isampler1DArray";		break;
	case GL_INT_SAMPLER_2D_ARRAY:						return "isampler2DArray";		break;
	case GL_INT_SAMPLER_2D_MULTISAMPLE:					return "isampler2DMS";			break;
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:			return "isampler2DMSArray";		break;
	case GL_INT_SAMPLER_BUFFER:							return "isamplerBuffer";		break;
	case GL_INT_SAMPLER_2D_RECT:						return "isampler2DRect";		break;

	case GL_UNSIGNED_INT_SAMPLER_1D:					return "usampler1D";			break;
	case GL_UNSIGNED_INT_SAMPLER_2D:					return "usampler2D";			break;
	case GL_UNSIGNED_INT_SAMPLER_3D:					return "usampler1D";			break;
	case GL_UNSIGNED_INT_SAMPLER_CUBE:					return "usamplerCube";			break;
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				return "usampler1DArray";		break;
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:				return "usampler2DArray";		break;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:		return "usampler2DMS";			break;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return "usampler2DMSArray";		break;
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:				return "usamplerBuffer";		break;
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:				return "usampler2DRect";		break;

#if OPENGL_VERSION >=42
	case GL_IMAGE_1D:									return "image1D";				break;
	case GL_IMAGE_2D:									return "image2D";				break;
	case GL_IMAGE_3D:									return "image3D";				break;
	case GL_IMAGE_2D_RECT:								return "image2DRect";			break;
	case GL_IMAGE_CUBE:									return "imageCube";				break;
	case GL_IMAGE_BUFFER:								return "imageBuffer";			break;
	case GL_IMAGE_1D_ARRAY:								return "image1DArray";			break;
	case GL_IMAGE_2D_ARRAY:								return "image2DArray";			break;
	case GL_IMAGE_2D_MULTISAMPLE:						return "image2DMS";				break;
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:					return "image2DMSArray";		break;

	case GL_INT_IMAGE_1D:								return "iimage1D";				break;
	case GL_INT_IMAGE_2D:								return "iimage2D";				break;
	case GL_INT_IMAGE_3D:								return "iimage3D";				break;
	case GL_INT_IMAGE_2D_RECT:							return "iimage2DRect";			break;
	case GL_INT_IMAGE_CUBE:								return "iimageCube";			break;
	case GL_INT_IMAGE_BUFFER:							return "iimageBuffer";			break;
	case GL_INT_IMAGE_1D_ARRAY:							return "iimage1DArray";			break;
	case GL_INT_IMAGE_2D_ARRAY:							return "iimage2DArray";			break;
	case GL_INT_IMAGE_2D_MULTISAMPLE:					return "iimage2DMS";			break;
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:				return "iimage2DMSArray";		break;

	case GL_UNSIGNED_INT_IMAGE_1D:						return "uimage1D";				break;
	case GL_UNSIGNED_INT_IMAGE_2D:						return "uimage2D";				break;
	case GL_UNSIGNED_INT_IMAGE_3D:						return "uimage3D";				break;
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:					return "uimage2DRect";			break;
	case GL_UNSIGNED_INT_IMAGE_CUBE:					return "uimageCube";			break;
	case GL_UNSIGNED_INT_IMAGE_BUFFER:					return "uimageBuffer";			break;
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:				return "uimage1DArray";			break;
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:				return "uimage2DArray";			break;
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:			return "uimage2DMS";			break;
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:	return "uimage2DMSArray";		break;

	case GL_UNSIGNED_INT_ATOMIC_COUNTER:				return "atomic_uint";			break;
#endif
	default: return "unknown type"; break;
	}
}
