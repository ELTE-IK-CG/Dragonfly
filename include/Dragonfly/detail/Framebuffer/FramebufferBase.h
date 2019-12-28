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
	inline void bind() const
	{
		glViewport(_x, _y, _w, _h);
		glBindFramebuffer(GL_FRAMEBUFFER,_id);
	}
public:
	template<typename Prog> Prog& operator << (Prog& prog) &
	{

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

};

}

