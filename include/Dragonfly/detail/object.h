#pragma once
#include "../config.h"
//#include "static_helper.h"
#include <GL/glew.h>

//namespace for opengl base classes
namespace eltecg { namespace ogl { namespace base {
	
//https://www.khronos.org/opengl/wiki/OpenGL_Object
class Object
{
public:
	//1. Encapsulated data types
	//friend class details::StaticHelper;
	operator GLuint () const { return object_id; }
protected:
	//2. Functions
	Object() = default;
	~Object() = default;
	
	//conversion operator
protected:
	//3. Variables
	GLuint object_id = 0;
	
}; // Object class

} } } //namespace eltecg::ogl::base