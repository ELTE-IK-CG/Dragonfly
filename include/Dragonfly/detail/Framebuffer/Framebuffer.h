#pragma once
#include <tuple>
#include "../../config.h"
#include "../Texture/Texture.h"
#include "../Texture/Texture2D.h"
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

	constexpr unsigned _none_ = 1024;

	template<unsigned depth_= _none_, unsigned stencil_ = _none_, unsigned depthstencil_ = _none_>
	struct FBO_compile_data
	{
		static constexpr unsigned depth() { return depth_; };
		static constexpr unsigned stencil() { return stencil_; }
		static constexpr unsigned depthstencil() { return depthstencil_; }
		template<typename F> static constexpr void static_addition_check() {
			static_assert(depth() == _none_ || !has_depth_v<F>   && !has_depthstencil_v<F>, "An FBO cannot have two depth textures.");
			static_assert(stencil() == _none_ || !has_stencil_v<F> && !has_depthstencil_v<F>, "An FBO cannot have two stencil textures.");
			static_assert(depthstencil() == _none_ || !has_depthstencil_v<F>, "An FBO cannot have two depth-stencil textures, that is just too much!");
		}
		template<typename F, unsigned size> using _add_InternalFormat_t = FBO_compile_data<has_depth_v<F> ? size : depth_, has_stencil_v<F> ? size : stencil_, has_depthstencil_v<F> ? size : depthstencil_>;
	};

	template<int index, typename InternalFormat> void attach2BoundFbo(const Texture2D<InternalFormat>& tex);
	template<int index, typename InternalFormat> void attach2BoundFbo(const Renderbuffer<InternalFormat>& ren);
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

	template<int idx> static constexpr int calc_extra_spots_until();
	template<int idx> constexpr void attach_all_upto_idx_to_bound_fb_with_given_size();
	template<> constexpr void attach_all_upto_idx_to_bound_fb_with_given_size<-1>() {}

public:
	using Compile_Data = compile_data;

	FramebufferObject(GLuint id, std::tuple<Attachements...> &&attachements, GLuint width = 0, GLuint height = 0) : FramebufferBase(id, 0,0,width,height), _attachements(std::move(attachements)), _width(width), _height(height){}
	FramebufferObject() { glCreateFramebuffers(1, &_id); }
	~FramebufferObject() { if(_id != 0) glDeleteFramebuffers(1, &_id); }

	FramebufferObject(const FramebufferObject&) = delete;
	FramebufferObject(FramebufferObject&& _o): FramebufferBase(std::move(_o)), _attachements(std::move(_o._attachements)), _width(_o._width), _height(_o._height) { _o._id = 0; }

	FramebufferObject& operator= (const FramebufferObject&) = delete;
	FramebufferObject& operator= (FramebufferObject&& _o);

	template<typename InternalFormat> FramebufferObject(Texture2D<InternalFormat>&& tex) ;
	template<typename InternalFormat> FramebufferObject(const Texture2D<InternalFormat>& tex) : FramebufferObject(tex.MakeView(0_level)) {}
	template<typename InternalFormat> FramebufferObject(Renderbuffer<InternalFormat>&& ren);
	template<typename InternalFormat> FramebufferObject(const Renderbuffer<InternalFormat>& ren) = delete;
	
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (Texture2D<InternalFormat>&& tex) &&;
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (const Texture2D<InternalFormat> &tex) &&	{ return std::move(*this) + tex.MakeView(0_level);}

	template<typename InternalFormat> _add_Renderbuffer_t<InternalFormat> operator + (Renderbuffer<InternalFormat>&& ren) &&;
	template<typename InternalFormat> _add_Renderbuffer_t<InternalFormat> operator + (const Renderbuffer<InternalFormat> &ren) && = delete;

	template<int idx> constexpr typename auto& getColor();
	constexpr typename auto& getDepth();
	constexpr typename auto& getStencil();
	constexpr typename auto& getDepthStencil();

	FramebufferObject MakeResized(GLuint width, GLuint height) const;

	template<int idx> FramebufferObject& operator<< (const detail::ClearColorF<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorI<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorU<idx>& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthF& cleardata);
	FramebufferObject& operator<< (const detail::ClearStencilI& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthStencilIF& cleardata);

	using FramebufferBase::operator<<;
};


} // namespace df

#include "Framebuffer.inl"
