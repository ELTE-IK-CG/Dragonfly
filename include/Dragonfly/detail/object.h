#pragma once
#include "../config.h"
//#include "static_helper.h"
#include <GL/glew.h>

#include <utility>

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

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	Object(Object&& o) : object_id(o.object_id) { o.object_id = 0; }
	Object& operator=(Object&& o) {
		std::swap(object_id, o.object_id);
		return *this;
	}
	
	//conversion operator
protected:
	//3. Variables
	GLuint object_id = 0;
	
}; // Object class

} } } //namespace eltecg::ogl::base