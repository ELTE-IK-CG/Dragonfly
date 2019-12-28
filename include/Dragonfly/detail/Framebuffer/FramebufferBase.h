#pragma once
#include "../../config.h"
#include <GL/glew.h>

namespace df
{

class FramebufferBase
{
	friend class ProgramLowLevelBase;
protected:
	GLuint _id;
	GLint _x, _y;
	GLsizei _w, _h;
	FramebufferBase(GLuint id=0, GLint x=0, GLint y=0, GLsizei w=0, GLsizei h=0) :_id(id), _x(x),_y(y),_w(w),_h(h) {}
	inline void bind();

public:
	template<typename Prog> Prog& operator << (Prog& prog) &
	{
		prog.framebuffer = *this;
		return prog;
	}
	template<typename Prog> Prog& operator << (Prog& prog) &&
	{
		static_assert(false);
		return prog;
	}

};

class DefaultFramebuffer : public FramebufferBase
{
public:
	DefaultFramebuffer(GLint x, GLint y, GLsizei w, GLsizei h) : FramebufferBase(0, x, y, w, h) {}
	DefaultFramebuffer(GLsizei w, GLsizei h) : DefaultFramebuffer(0, 0, w, h) {}
};

inline DefaultFramebuffer Canvas(0, 0);

inline void FramebufferBase::bind()
{
	if (_id == 0 && _w == 0) {
		*this = Canvas;
	}
	glViewport(_x, _y, _w, _h);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

}

