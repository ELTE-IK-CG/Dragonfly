#pragma once
#include "../../config.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace df
{
	namespace detail {
		template<int index = 0> struct ClearColorF { float _red, _green, _blue, _alpha; };
		template<int index = 0> struct ClearColorI { int _red, _green, _blue, _alpha; };
		template<int index = 0> struct ClearColorU { unsigned _red, _green, _blue, _alpha; };
		struct ClearDepthF { float _depth; };
		struct ClearStencilI { int _stencil; };
		struct ClearDepthStencilIF { float _depth;  int _stencil; };

		template<int index = 0> struct ClearF { ClearColorF<index> color; float _depth; int _stencil; };
	}
	template<int index = 0>	detail::ClearColorF<index> ClearColor(float allchannels = 0.f) { return { allchannels, allchannels, allchannels, allchannels }; }
	template<int index = 0>	detail::ClearColorF<index> ClearColor(float red, float green, float blue, float alpha = 1.f) { return { red, green, blue, alpha }; }
	template<int index = 0> detail::ClearColorI<index> ClearColor(int red, int green, int blue, int alpha) { return { red, green, blue, alpha }; }
	template<int index = 0> detail::ClearColorU<index> ClearColor(unsigned red, unsigned green, unsigned blue, unsigned alpha) { return { red, green, blue, alpha }; }
	inline detail::ClearDepthF ClearDepth(float depth = 1.f) { return { depth }; }
	inline detail::ClearStencilI ClearStencil(int stencil = 0) { return { stencil }; }
	inline detail::ClearDepthStencilIF ClearDepthStencil(float depth = 1.f, int stencil = 0) { return {depth, stencil }; }

	template<int index = 0> detail::ClearF<index> Clear(float red, float green, float blue, float alpha = 1.f, float depth = 1.f, int stencil = 0) { return { ClearColor(red, green, blue, alpha), depth, stencil }; }
	template<int index = 0> detail::ClearF<index> Clear(float allchannels = 0.f, float depth = 1.f, int stencil = 0) { return {ClearColor(allchannels), depth, stencil}; }

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
	template<typename Prog> Prog& operator << (Prog& prog) &;
	template<typename Prog> Prog& operator << (Prog& prog) && = delete;

	explicit operator GLuint() const { return _id; }
	
	glm::ivec2 getSize() const { return { _w,_h }; }
	GLsizei getWidth() const { return _w; }
	GLsizei getHeight() const { return _h; }
	GLint getX() const { return _x; }
	GLint getY() const { return _y; }
};

class DefaultFramebuffer : public FramebufferBase
{
public:
	DefaultFramebuffer(GLint x, GLint y, GLsizei w, GLsizei h) : FramebufferBase(0, x, y, w, h) {}
	DefaultFramebuffer(GLsizei w, GLsizei h) : DefaultFramebuffer(0, 0, w, h) {}
	DefaultFramebuffer& operator<< (const detail::ClearColorF<0> & cleardata) { this->bind(); glClearBufferfv(GL_COLOR, 0, &cleardata._red); return *this; }
	DefaultFramebuffer& operator<< (const detail::ClearDepthF& cleardata) { this->bind(); glClearBufferfv(GL_DEPTH, 0, &cleardata._depth); return *this; }
	DefaultFramebuffer& operator<< (const detail::ClearStencilI& cleardata) { this->bind(); glClearBufferiv(GL_DEPTH, 0, &cleardata._stencil); return *this; }
	DefaultFramebuffer& operator<< (const detail::ClearDepthStencilIF& cleardata) { this->bind(); glClearBufferfi(GL_DEPTH_STENCIL, 0, cleardata._depth, cleardata._stencil); return *this; }
	DefaultFramebuffer& operator<< (const detail::ClearF<0> & cleardata) { this->bind(); glClearBufferfv(GL_COLOR, 0, &cleardata.color._red); glClearBufferfi(GL_DEPTH_STENCIL, 0, cleardata._depth, cleardata._stencil); return *this; }

	using FramebufferBase::operator<<;

	void HandleResize(int w, int h);
};

inline DefaultFramebuffer Backbuffer(0, 0);

inline void FramebufferBase::bind() {
	if (_id == 0 && _w == 0) {
		*this = Backbuffer;
	}
	glViewport(_x, _y, _w, _h);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

template<typename Prog> Prog& FramebufferBase::operator<<(Prog& prog) & {
	prog.framebuffer = *this;
	return prog;
}

inline void DefaultFramebuffer::HandleResize(int w, int h)
{
	this->_w = w;
	this->_h = h;
	this->bind();
	glViewport(this->_x, this->_y, this->_w, this->_h);
}

}

