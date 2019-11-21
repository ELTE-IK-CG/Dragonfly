#pragma once
#include "../../config.h"
#include <tuple>
#include "../Texture/Texture.h"
#include "../Texture/Texture2D.h"
#include "../Texture/TextureCube.h"

namespace df
{

class FrameBufferBase
{

};

class DefaultFramebuffer :public FrameBufferBase
{

};


template<int i>
struct Ind
{

};


template<typename InternalFormat = eltecg::ogl::helper::depth24>
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
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glRenderbufferStorage(GL_RENDERBUFFER, iFormat, width, height);
	}
};

template<typename ... Attachements>
class FrameBufferObject
{
	std::tuple<Attachements...> attachements;
	constexpr bool has_depth = false;
	constexpr bool has_stencil = false;
	constexpr bool has_depthstencil = false;
public:

	template<int i>
	constexpr auto& get() {		//static_assert()
	 	return std::get<i>(attachements);
	}
};

template<typename ... ColorAttachements>
using FBO = FrameBufferObject<Renderbuffer<>, ColorAttachements...>

}

