#pragma once

#include "config.h"
#include <GL/glew.h>

namespace df
{

class VaoBase {
public:
	GLuint _id;
	GLenum _mode;
	GLenum _count;
	GLenum _ibo_type = 0;
	GLsizei _first = 0;
};

class VaoWrapper : public VaoBase {
public:
	VaoWrapper(GLuint id, GLenum mode, GLsizei count, GLenum ibo_type = 0u) { this->_id = id; this->_mode = mode; this->_ibo_type = ibo_type; this->_count = count; }
	VaoWrapper(GLuint id, GLenum mode, GLsizei count, GLint first = 0) { this->_id = id; this->_mode = mode; this->_first = first; this->_count = count; }
};

}
