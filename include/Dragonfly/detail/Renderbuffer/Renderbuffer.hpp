#pragma once
#include "../../config.h"
#include "../Traits/InternalFormats.h"
#include <iostream>

namespace df
{

template<typename InternalFormat>
class Renderbuffer
{
	GLuint _id;
	GLsizei _w, _h;
	
	void bind() { glBindRenderbuffer(GL_RENDERBUFFER, _id); }
public:
	using PixelFormat = InternalFormat;

	Renderbuffer(GLsizei w, GLsizei h)
		: _w(w), _h(h)
	{
		glGenRenderbuffers(1, &_id);
		this->bind();
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat>();
		glRenderbufferStorage(GL_RENDERBUFFER, iFormat, w, h);
	}
	~Renderbuffer() { glDeleteRenderbuffers(1, &_id); }
	
	Renderbuffer(const Renderbuffer&) = delete;
	Renderbuffer(Renderbuffer&& _o)
		: _id(_o._id), _w(_o._w), _h(_o._h)
	{
		_o._id = 0;
	}

	Renderbuffer& operator= (const Renderbuffer&) = delete;
	Renderbuffer& operator= (Renderbuffer&& _o)
	{
		std::swap(_id, _o._id);
		_w = _o._w;
		_h = _o._h;
		return *this;
	}

	explicit operator GLuint() const { return _id; }

	GLsizei getWidth() const { return _w; }
	GLsizei getHeight() const { return _h; }

	Renderbuffer MakeResized(GLuint width, GLuint height) const { return Renderbuffer(width, height); }
};

}

