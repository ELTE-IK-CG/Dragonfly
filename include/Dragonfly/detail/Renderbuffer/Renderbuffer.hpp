#pragma once
#include "../../config.h"
#include <iostream>

namespace df
{

template<typename InternalFormat>
class Renderbuffer
{
	GLuint _id;
	bool inited = false;
	
	void bind() { glBindRenderbuffer(GL_RENDERBUFFER, _id); }
public:
	using PixelFormat = InternalFormat;
	Renderbuffer() { glGenRenderbuffers(1, &_id); }
	Renderbuffer(GLsizei w, GLsizei h) { glGenRenderbuffers(1, &_id); Init(w, h); }
	~Renderbuffer() { glDeleteRenderbuffers(1, &_id); }
	
	void Init(GLsizei width, GLsizei height)
	{
		this->bind();
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat>();
		glRenderbufferStorage(GL_RENDERBUFFER, iFormat, width, height);
	}
};

}

