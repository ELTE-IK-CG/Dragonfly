#include "UniformTypes.hpp"

using namespace df;

OpenGL_BaseType::ValueType OpenGL_BaseType::GetCppTypeFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:										return GLfloat();	
	case GL_FLOAT_VEC2:									return glm::vec2();	
	case GL_FLOAT_VEC3:									return glm::vec3();	
	case GL_FLOAT_VEC4:									return glm::vec4();	
	case GL_DOUBLE:										return GLdouble();	
	case GL_DOUBLE_VEC2:								return glm::dvec2();
	case GL_DOUBLE_VEC3:								return glm::dvec3();
	case GL_DOUBLE_VEC4:								return glm::dvec4();

	case GL_INT:										return GLint();		
	case GL_INT_VEC2:									return glm::ivec2();
	case GL_INT_VEC3:									return glm::ivec3();
	case GL_INT_VEC4:									return glm::ivec4();
	case GL_UNSIGNED_INT:								return GLuint();	
	case GL_UNSIGNED_INT_VEC2:							return glm::uvec2();
	case GL_UNSIGNED_INT_VEC3:							return glm::uvec3();
	case GL_UNSIGNED_INT_VEC4:							return glm::uvec4();

	case GL_FLOAT_MAT2:									return glm::mat2();	
	case GL_FLOAT_MAT3:									return glm::mat3();	
	case GL_FLOAT_MAT4:									return glm::mat4();	
	case GL_FLOAT_MAT2x3:								return glm::mat2x3();
	case GL_FLOAT_MAT3x2:								return glm::mat3x2();
	case GL_FLOAT_MAT2x4:								return glm::mat2x4();
	case GL_FLOAT_MAT4x2:								return glm::mat4x2();
	case GL_FLOAT_MAT3x4:								return glm::mat3x4();
	case GL_FLOAT_MAT4x3:								return glm::mat4x3();

	case GL_DOUBLE_MAT2:								return glm::dmat2();
	case GL_DOUBLE_MAT3:								return glm::dmat3();
	case GL_DOUBLE_MAT4:								return glm::dmat4();
	case GL_DOUBLE_MAT2x3:								return glm::dmat2x3();
	case GL_DOUBLE_MAT3x2:								return glm::dmat3x2();
	case GL_DOUBLE_MAT2x4:								return glm::dmat2x4();
	case GL_DOUBLE_MAT4x2:								return glm::dmat4x2();
	case GL_DOUBLE_MAT3x4:								return glm::dmat3x4();
	case GL_DOUBLE_MAT4x3:								return glm::dmat4x3();

	default: return UniformLowLevelBase::uni_hash_type(0); break;
	}
}


std::string OpenGL_BaseType::GetTypeNameFromOpenGlType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:										return "float";				
	case GL_FLOAT_VEC2:									return "vec2";				
	case GL_FLOAT_VEC3:									return "vec3";				
	case GL_FLOAT_VEC4:									return "vec4";				
	case GL_INT:										return "int";				
	case GL_INT_VEC2:									return "ivec2";				
	case GL_INT_VEC3:									return "ivec3";				
	case GL_INT_VEC4:									return "ivec4";				
	case GL_UNSIGNED_INT:								return "uint";				
	case GL_UNSIGNED_INT_VEC2:							return "uvec2";				
	case GL_UNSIGNED_INT_VEC3:							return "uvec3";				
	case GL_UNSIGNED_INT_VEC4:							return "uvec4";				
	
	case GL_FLOAT_MAT2:									return "mat2";				
	case GL_FLOAT_MAT3:									return "mat3";				
	case GL_FLOAT_MAT4:									return "mat4";				
	case GL_FLOAT_MAT2x3:								return "mat2x3";			
	case GL_FLOAT_MAT3x2:								return "mat3x2";			
	case GL_FLOAT_MAT2x4:								return "mat2x4";			
	case GL_FLOAT_MAT4x2:								return "mat4x2";			
	case GL_FLOAT_MAT3x4:								return "mat3x4";			
	case GL_FLOAT_MAT4x3:								return "mat4x3";			

#if OPENGL_VERSION >=41	
	case GL_DOUBLE:										return "double";			
	case GL_DOUBLE_VEC2:								return "dvec2";				
	case GL_DOUBLE_VEC3:								return "dvec3";				
	case GL_DOUBLE_VEC4:								return "dvec4";				

	case GL_DOUBLE_MAT2:								return "dmat2";				
	case GL_DOUBLE_MAT3:								return "dmat3";				
	case GL_DOUBLE_MAT4:								return "dmat4";				
	case GL_DOUBLE_MAT2x3:								return "dmat2x3";			
	case GL_DOUBLE_MAT3x2:								return "dmat3x2";			
	case GL_DOUBLE_MAT2x4:								return "dmat2x4";			
	case GL_DOUBLE_MAT4x2:								return "dmat4x2";			
	case GL_DOUBLE_MAT3x4:								return "dmat3x4";			
	case GL_DOUBLE_MAT4x3:								return "dmat4x3";			
#endif
	case GL_SAMPLER_1D:									return "sampler1D";			
	case GL_SAMPLER_2D:									return "sampler2D";			
	case GL_SAMPLER_3D:									return "sampler3D";			
	case GL_SAMPLER_CUBE:								return "samplerCube";		
	case GL_SAMPLER_1D_SHADOW:							return "sampler1DShadow";	
	case GL_SAMPLER_2D_SHADOW:							return "sampler2DShadow";	
	case GL_SAMPLER_1D_ARRAY:							return "sampler1DArray";	
	case GL_SAMPLER_2D_ARRAY:							return "sampler2DArray";	
	case GL_SAMPLER_1D_ARRAY_SHADOW:					return "sampler1DArrayShadow";
	case GL_SAMPLER_2D_ARRAY_SHADOW:					return "sampler2DArrayShadow";
	case GL_SAMPLER_2D_MULTISAMPLE:						return "sampler2DMS";		
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:				return "sampler2DMSArray";	
	case GL_SAMPLER_CUBE_SHADOW:						return "samplerCubeShadow";	
	case GL_SAMPLER_BUFFER:								return "samplerBuffer";		
	case GL_SAMPLER_2D_RECT:							return "sampler2DRect";		
	case GL_SAMPLER_2D_RECT_SHADOW:						return "sampler2DRectShadow";

	case GL_INT_SAMPLER_1D:								return "isampler1D";		
	case GL_INT_SAMPLER_2D:								return "isampler2D";		
	case GL_INT_SAMPLER_3D:								return "isampler3D";		
	case GL_INT_SAMPLER_CUBE:							return "isamplerCube";		
	case GL_INT_SAMPLER_1D_ARRAY:						return "isampler1DArray";	
	case GL_INT_SAMPLER_2D_ARRAY:						return "isampler2DArray";	
	case GL_INT_SAMPLER_2D_MULTISAMPLE:					return "isampler2DMS";		
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:			return "isampler2DMSArray";	
	case GL_INT_SAMPLER_BUFFER:							return "isamplerBuffer";	
	case GL_INT_SAMPLER_2D_RECT:						return "isampler2DRect";	

	case GL_UNSIGNED_INT_SAMPLER_1D:					return "usampler1D";		
	case GL_UNSIGNED_INT_SAMPLER_2D:					return "usampler2D";		
	case GL_UNSIGNED_INT_SAMPLER_3D:					return "usampler3D";		
	case GL_UNSIGNED_INT_SAMPLER_CUBE:					return "usamplerCube";		
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				return "usampler1DArray";	
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:				return "usampler2DArray";	
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:		return "usampler2DMS";		
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return "usampler2DMSArray";	
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:				return "usamplerBuffer";	
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:				return "usampler2DRect";	

#if OPENGL_VERSION >=42
	case GL_IMAGE_1D:									return "image1D";			
	case GL_IMAGE_2D:									return "image2D";			
	case GL_IMAGE_3D:									return "image3D";			
	case GL_IMAGE_2D_RECT:								return "image2DRect";		
	case GL_IMAGE_CUBE:									return "imageCube";			
	case GL_IMAGE_BUFFER:								return "imageBuffer";		
	case GL_IMAGE_1D_ARRAY:								return "image1DArray";		
	case GL_IMAGE_2D_ARRAY:								return "image2DArray";		
	case GL_IMAGE_2D_MULTISAMPLE:						return "image2DMS";			
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:					return "image2DMSArray";	

	case GL_INT_IMAGE_1D:								return "iimage1D";			
	case GL_INT_IMAGE_2D:								return "iimage2D";			
	case GL_INT_IMAGE_3D:								return "iimage3D";			
	case GL_INT_IMAGE_2D_RECT:							return "iimage2DRect";		
	case GL_INT_IMAGE_CUBE:								return "iimageCube";		
	case GL_INT_IMAGE_BUFFER:							return "iimageBuffer";		
	case GL_INT_IMAGE_1D_ARRAY:							return "iimage1DArray";		
	case GL_INT_IMAGE_2D_ARRAY:							return "iimage2DArray";		
	case GL_INT_IMAGE_2D_MULTISAMPLE:					return "iimage2DMS";		
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:				return "iimage2DMSArray";	

	case GL_UNSIGNED_INT_IMAGE_1D:						return "uimage1D";			
	case GL_UNSIGNED_INT_IMAGE_2D:						return "uimage2D";			
	case GL_UNSIGNED_INT_IMAGE_3D:						return "uimage3D";			
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:					return "uimage2DRect";		
	case GL_UNSIGNED_INT_IMAGE_CUBE:					return "uimageCube";		
	case GL_UNSIGNED_INT_IMAGE_BUFFER:					return "uimageBuffer";		
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:				return "uimage1DArray";		
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:				return "uimage2DArray";		
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:			return "uimage2DMS";		
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:	return "uimage2DMSArray";	

	case GL_UNSIGNED_INT_ATOMIC_COUNTER:				return "atomic_uint";		

	//was missing from https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
	case GL_SAMPLER_CUBE_MAP_ARRAY:						return "samplerCubeArray";				 //0x900C
	case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:				return "samplerCubeArrayShadow";		 //0x900D
	case GL_INT_SAMPLER_CUBE_MAP_ARRAY:					return "isamplerCubeArray";				 //0x900E
	case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:		return "usamplerCubeArrayShadow";		 //0x900F
	case GL_IMAGE_CUBE_MAP_ARRAY:						return "imageCubeArray";		
	case GL_INT_IMAGE_CUBE_MAP_ARRAY:					return "iimageCubeArray";		
	case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:			return "uimageCubeArray";		
#endif
	default: return "unknown type"; break;
	}
}

bool df::isOpenGLTextureType(GLenum type)
{
	switch (type)
	{
	case GL_SAMPLER_1D:								case GL_SAMPLER_2D:									case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:							case GL_SAMPLER_1D_SHADOW:							case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:						case GL_SAMPLER_2D_ARRAY:							case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:				case GL_SAMPLER_2D_MULTISAMPLE:						case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:					case GL_SAMPLER_BUFFER:								case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:							case GL_INT_SAMPLER_2D:								case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:						case GL_INT_SAMPLER_1D_ARRAY:						case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:				case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:			case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:				case GL_UNSIGNED_INT_SAMPLER_2D:					case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:				case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
#if OPENGL_VERSION >=42
	case GL_IMAGE_1D:								case GL_IMAGE_2D:									case GL_IMAGE_3D:
	case GL_IMAGE_2D_RECT:							case GL_IMAGE_CUBE:									case GL_IMAGE_BUFFER:
	case GL_IMAGE_1D_ARRAY:							case GL_IMAGE_2D_ARRAY:								case GL_IMAGE_2D_MULTISAMPLE:
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_INT_IMAGE_1D:							case GL_INT_IMAGE_2D:								case GL_INT_IMAGE_3D:
	case GL_INT_IMAGE_2D_RECT:						case GL_INT_IMAGE_CUBE:								case GL_INT_IMAGE_BUFFER:
	case GL_INT_IMAGE_1D_ARRAY:						case GL_INT_IMAGE_2D_ARRAY:							case GL_INT_IMAGE_2D_MULTISAMPLE:
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_1D:					case GL_UNSIGNED_INT_IMAGE_2D:						case GL_UNSIGNED_INT_IMAGE_3D:
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:				case GL_UNSIGNED_INT_IMAGE_CUBE:					case GL_UNSIGNED_INT_IMAGE_BUFFER:
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:			case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:				case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_ATOMIC_COUNTER:
	case GL_SAMPLER_CUBE_MAP_ARRAY:					case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:				case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
	case GL_IMAGE_CUBE_MAP_ARRAY:					case GL_INT_IMAGE_CUBE_MAP_ARRAY:					case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:
#endif
		return true;
	default:
		return false;
	}
}