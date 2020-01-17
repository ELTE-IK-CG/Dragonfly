#pragma once
#include "../../config.h"
#include "../Traits/InternalFormats.h"
#include <iostream>

namespace df
{

template<typename InternalFormat_>
class Renderbuffer
{
public:
	using Internal_Format = InternalFormat_;

private:
	GLuint _id;
	GLsizei _w, _h;

private:
	void bind() { glBindRenderbuffer(GL_RENDERBUFFER, _id); }

public:
	Renderbuffer(GLsizei w, GLsizei h);
	~Renderbuffer() { glDeleteRenderbuffers(1, &_id); }
	
	Renderbuffer(const Renderbuffer&) = delete;
	Renderbuffer(Renderbuffer&& _o);

	Renderbuffer& operator= (const Renderbuffer&) = delete;
	Renderbuffer& operator= (Renderbuffer&& _o);

	GLsizei getWidth() const { return _w; }
	GLsizei getHeight() const { return _h; }

	explicit operator GLuint() const { return _id; }

	Renderbuffer MakeResized(GLuint width, GLuint height) const { return Renderbuffer(width, height); }
};

template<typename InternalFormat_>
Renderbuffer<InternalFormat_>::Renderbuffer(GLsizei w, GLsizei h)
	: _w(w), _h(h)
{
	glGenRenderbuffers(1, &_id);
	this->bind();
	constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat_>();
	glRenderbufferStorage(GL_RENDERBUFFER, iFormat, w, h);
}

template<typename InternalFormat_>
Renderbuffer<InternalFormat_>::Renderbuffer(Renderbuffer&& _o)
	: _id(_o._id), _w(_o._w), _h(_o._h) {
	_o._id = 0;
}

template<typename InternalFormat_>
Renderbuffer<InternalFormat_>& Renderbuffer<InternalFormat_>::operator=(Renderbuffer&& _o)
{
	std::swap(_id, _o._id);
	_w = _o._w;
	_h = _o._h;
	return *this;
}

}

