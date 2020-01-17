#pragma once

#include "../../config.h"
#include <GL/glew.h>

namespace df
{


struct VaoBase
{
	const GLuint _id;
	const GLenum _mode;
	const GLenum _count;
	void bind() const {glBindVertexArray(_id);}
	VaoBase(GLuint id, GLenum mode, GLsizei count) : _id(id), _mode(mode), _count(count) {}
};

struct VaoElements : public VaoBase
{
	const GLenum _ibo;
	VaoElements(GLuint id, GLenum mode, GLsizei count, GLenum ibo_elem_type) : VaoBase(id, mode, count), _ibo(ibo_elem_type) {}
};

struct VaoArrays : public VaoBase
{
	const GLint _first;
	VaoArrays(GLuint id, GLenum mode, GLsizei count, GLint first) : VaoBase(id, mode, count), _first(first) {}
};

struct NoVao : public VaoArrays
{
	NoVao(GLenum mode, GLsizei count, GLint first = 0) : VaoArrays(0, mode, count, first) {}
};


}
