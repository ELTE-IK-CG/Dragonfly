#pragma once
#include <tuple>
#include "../../config.h"
#include "../Texture/Texture.h"
#include "../Texture/Texture2D.h"
#include "../Texture/TextureCube.h"
#include "../Framebuffer/FramebufferBase.h"
#include "../Renderbuffer/Renderbuffer.hpp"
#include <iostream>

namespace df
{

namespace detail
{
	template<typename F> constexpr bool has_depth_v				= std::is_same_v<F, depth16> || std::is_same_v<F, depth24> || std::is_same_v<F, depth32F>;
	template<typename F> constexpr bool has_stencil_v			= std::is_same_v<F, stencil1> || std::is_same_v<F, stencil4> || std::is_same_v<F, stencil8> || std::is_same_v<F, stencil16>;
	template<typename F> constexpr bool has_depthstencil_v		= std::is_same_v<F, depth24stencil8> || std::is_same_v<F, depth32Fstencil8 >;
	template<typename F> constexpr bool is_color_attachement_v	= !has_depth_v<F> && !has_stencil_v<F> && !has_depthstencil_v<F>;
	template<typename F> constexpr GLenum get_attachement_v		= has_depth_v<F> ? GL_DEPTH_ATTACHMENT : has_stencil_v<F> ? GL_STENCIL_ATTACHMENT : has_depthstencil_v<F> ? GL_DEPTH_STENCIL_ATTACHMENT : 0;

	template<unsigned depth_= -1, unsigned stencil_ = -1, unsigned depthstencil_ = -1>
	struct FBO_compile_data
	{
		static constexpr unsigned depth() { return depth_; };
		static constexpr unsigned stencil() { return stencil_; }
		static constexpr unsigned depthstencil() { return depthstencil_; }
		template<typename F> static constexpr void static_addition_check()
		{
			static_assert(depth() == -1        || !has_depth_v<F>   && !has_depthstencil_v<F>, "An FBO cannot have two depth textures.");
			static_assert(stencil() == -1      || !has_stencil_v<F> && !has_depthstencil_v<F>, "An FBO cannot have two stencil textures.");
			static_assert(depthstencil() == -1 || !has_depthstencil_v<F>, "An FBO cannot have two depth-stencil textures, that is just too much!");
		}
		template<typename F, unsigned size> using _add_InternalFormat_t = FBO_compile_data<has_depth_v<F> ? size : -1, has_stencil_v<F> ? size : -1, has_depthstencil_v<F> ? size : -1>;
	};
}

template<typename compile_data, typename ... Attachements>
class FramebufferObject : public FramebufferBase
{
private:
	//namespace cg = eltecg::ogl::helper;
	std::tuple<Attachements...> _attachements;
	GLuint _width = 0, _height = 0;
private:

	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, _id); }

	template<typename F> using _add_Texture2D_t    = FramebufferObject < typename compile_data:: template _add_InternalFormat_t<F, sizeof...(Attachements)>, Attachements..., Texture2D<F>>;
	template<typename F> using _add_Renderbuffer_t = FramebufferObject < typename compile_data:: template _add_InternalFormat_t<F, sizeof...(Attachements)>, Attachements..., Renderbuffer<F>>;

	template<int idx>
	static constexpr int calc_extra_spots_until()
	{
		static_assert(idx >= 0 && idx <= sizeof...(Attachements), "Index out of bounds here.");
		return (idx > compile_data::depth() ? 1 : 0) + (idx > compile_data::stencil() ? 1 : 0) + (idx > compile_data::depthstencil() ? 1 : 0);
	}

public:
	FramebufferObject(GLuint id, std::tuple<Attachements...> &&attachements, GLuint width = 0, GLuint height = 0) : FramebufferBase(id, 0,0,width,height), _attachements(std::move(attachements)), _width(width), _height(height){}
	FramebufferObject() { glCreateFramebuffers(1, &_id); }
	~FramebufferObject() { if(_id != 0) glDeleteFramebuffers(1, &_id); }
	FramebufferObject(FramebufferObject&&) = default;

	template<typename InternalFormat> FramebufferObject(Texture2D<InternalFormat>     && tex) : FramebufferBase(0, 0, 0, tex.getWidth(), tex.getHeight()), _attachements(std::make_tuple(std::move(tex))), _width(tex.getWidth()), _height(tex.getHeight()) { glCreateFramebuffers(1, &this->_id); }
	template<typename InternalFormat> FramebufferObject(const Texture2D<InternalFormat>& tex) : FramebufferObject(tex.MakeView(0_level)){}
	//FramebufferObject& operator=(FramebufferObject&&) = default;
	
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (Texture2D<InternalFormat>&& tex) &&;
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (const Texture2D<InternalFormat> &tex) &&	{ return std::move(*this) + tex.MakeView(0_level);}

	template<int idx>
	constexpr typename std::tuple_element<idx, std::tuple<Attachements...>>& get()
	{
		constexpr int index = idx + calc_extra_spots_until<idx>();
		static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
		return std::get< index >(_attachements);
	}
};

using EmptyFBO = FramebufferObject<detail::FBO_compile_data<>>;

template<typename ... ColorAttachements>
using FBO = FramebufferObject<Renderbuffer<depth24>, ColorAttachements...>;

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>::operator FramebufferObject<detail::FBO_compile_data<-1, -1, -1>, Texture<TextureType::TEX_2D, InternalFormat>>() &&
{
	return FramebufferObject<detail::FBO_compile_data<-1, -1, -1>, Texture<TextureType::TEX_2D, InternalFormat>>(std::move(*this));
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>::operator FramebufferObject<detail::FBO_compile_data<-1, -1, -1>, Texture<TextureType::TEX_2D, InternalFormat>>() const&
{
	return FramebufferObject<detail::FBO_compile_data<-1, -1, -1>, Texture<TextureType::TEX_2D, InternalFormat>>(*this);
}

template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
inline FramebufferObject<compile_data, Attachements...>::_add_Texture2D_t<InternalFormat> FramebufferObject<compile_data, Attachements...>::operator+(Texture2D<InternalFormat>&& tex) &&
{
	compile_data::template static_addition_check<InternalFormat>();

	constexpr int idx = sizeof...(Attachements);
	constexpr int index = idx - calc_extra_spots_until<idx>();
	static_assert(index >= 0 && index <= sizeof...(Attachements), "Index out of bounds here.");
	constexpr GLenum attachement = detail::is_color_attachement_v<InternalFormat> ? GL_COLOR_ATTACHMENT0 + index : detail::get_attachement_v<InternalFormat>;

	ASSERT((this->_width == 0 || tex.getWidth() == 0 || this->_width == tex.getWidth()) && (this->_height == 0 || tex.getHeight() == 0 || this->_height == tex.getHeight()), "Unmaching texture size in framebuffer object.");
	int w = (this->_width == 0 ? tex.getWidth() : this->_width), h = (this->_height == 0 ? tex.getHeight() : this->_height);

	this->bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachement, GL_TEXTURE_2D, (GLuint)tex, 0);

	this->_id = 0;
	std::cout << "w = " << w << " h = " << h << std::endl;
	return _add_Texture2D_t<InternalFormat>(this->_id, std::tuple_cat(std::move(this->_attachements), std::make_tuple(std::move(tex))), w, h);
}

}

