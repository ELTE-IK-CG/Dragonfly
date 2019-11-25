#pragma once
#include <tuple>
#include "../../config.h"
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

namespace detail {

	template<int depth_=-1, int stencil_=-1, int depthstencil_= -1>
	struct FBO_compile_data{
		constexpr int depth = depth_;			constexpr int stencil = stencil_;			constexpr int depthstencil = depthstencil_;
		constexpr bool has_depth = depth != -1;	constexpr bool has_stencil = stencil != -1;	constexpr bool has_depthstencil = depthstencil != -1;
	};
}

template<typename compile_data, typename ... Attachements>
class FramebufferObject : public FrameBufferBase
{
private:
	std::tuple<Attachements...> _attachements;
	GLuint _id;
	namespace cg = eltecg::ogl::helper;
private:
	FramebufferObject(GLuint id, std::tuple<Attachements...> &&attachements):_id(id), _attachements(attachements){}

	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, _id); }

	/*template<typename new_compile_data, typename ... NewAttachements>
	using CombinedFramebuffer_t = FramebufferObject<detail::FBO_compile_data<
		compile_data::has_depth			| new_compile_data::has_depth,
		compile_data::has_stencil		| new_compile_data::has_stencil,
		compile_data::has_depthstencil	| new_compile_data::has_depthstencil
	>, Attachements..., NewAttachements...>;*/

	template<typename F>
	using InternalFormat_AddT = detail::FBO_compile_data<
		std::is_same_v<F, cg::depth16>			|| std::is_same_v<F, cg::depth32>		|| std::is_same_v<F, cg::depth32F>,
		std::is_same_v<F, cg::stencil1>			|| std::is_same_v<F, cg::stencil4>		|| std::is_same_v<F, cg::stencil8>	|| std::is_same_v<F, cg::stencil16>,
		std::is_same_v<F, cg::depth24stencil8>	|| std::is_same_v<F, cg::depth32Fstencil8>>;

	template<typename F>
	using FramebufferObject_add_Texture2D = FramebufferObject < InternalFormat_AddT<F>, Attachements..., Texture2D<F>>;

	template<typename F>
	using FramebufferObject_add_Renderbuffer = FramebufferObject < InternalFormat_AddT<F>, Attachements..., Renderbuffer<F>>;
	
	constexpr int calc_tuple_idx(int i)
	{
		//todo static assert
		return i + (i > compile_data::depth ? +1 : 0) + (i > compile_data::stencil ? +1 : 0) + (i > compile_data::depthstencil ? +1 : 0);
	}

public:
	FramebufferObject() { glCreateFramebuffers(1, &_id); }
	~FramebufferObject() { if(id != 0) glDeleteFramebuffers(1, &_id); }

	/*template<typename new_compile_data, typename ... NewAttachements>
	CombinedFramebuffer_t< new_compile_data, NewAttachements...>
		operator + (const FramebufferObject<NewAttachements...>& o)
	{
		CombinedFramebuffer_t< new_compile_data, NewAttachements...> fbo(this->_id);
		fbo.attache_attachementsments = std::tuple_cat(this->_attachements, o._attachements);
		_id = 0;
		return fbo;
	}*/

	template<typename InternalFormat>
	FramebufferObject_add_Texture2D<InternalFormat> operator + (const Texture2D<InternalFormat> &tex)
	{
		FramebufferObject_add_Texture2D<InternalFormat> fbo(this->_id, std::tuple_cat(this->_attachements, std::make_tuple(tex));
		this->bind();
		
		GLenum attachement = std::

		glFramebufferTexture2D(GL_FRAMEBUFFER, )

		this->_id = 0;
		return fbo;
	}

	template<int i>
	constexpr typename std::tuple_element<i, std::tuple<Attachements...>>& get() {		//static_assert()
		static_assert(0 <= i && i < std::tuple_size_v<std::tuple<Attachements...>>, "FramebufferObject: the index is not within the tuple's range.")
	 	return std::get<i>(_attachements);
	}
};

template<typename ... ColorAttachements>
using FBO = FramebufferObject<Renderbuffer<>, ColorAttachements...>;

}

