#pragma once
#include <tuple>
#include "../../config.h"
#include "../Texture/Texture.h"
#include "../Texture/Texture2D.h"
#include "../Framebuffer/FramebufferBase.h"
#include "../Renderbuffer/Renderbuffer.hpp"

namespace df
{

template<typename compile_data, typename ... Attachements>
class FramebufferObject : public FramebufferBase
{
private:
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

	template<typename InternalFormat_> FramebufferObject(Texture2D<InternalFormat_>&& tex) ;
	template<typename InternalFormat_> FramebufferObject(const Texture2D<InternalFormat_>& tex) : FramebufferObject(tex.MakeView(0_level)) {}
	template<typename InternalFormat_> FramebufferObject(Renderbuffer<InternalFormat_>&& ren);
	template<typename InternalFormat_> FramebufferObject(const Renderbuffer<InternalFormat_>& ren) = delete;
	
	template<typename InternalFormat_> _add_Texture2D_t   <InternalFormat_> operator + (Texture2D<InternalFormat_>&& tex) &&;
	template<typename InternalFormat_> _add_Texture2D_t   <InternalFormat_> operator + (const Texture2D<InternalFormat_> &tex) &&	{ return std::move(*this) + tex.MakeView(0_level);}
	template<typename InternalFormat_> _add_Renderbuffer_t<InternalFormat_> operator + (Renderbuffer<InternalFormat_>&& ren) &&;
	template<typename InternalFormat_> _add_Renderbuffer_t<InternalFormat_> operator + (const Renderbuffer<InternalFormat_> &ren) && = delete;

	template<typename InternalFormat_> constexpr Texture2D<InternalFormat_>& get();
	template<int idx> constexpr typename auto& getColor();
	constexpr typename auto& getDepth();
	constexpr typename auto& getStencil();
	constexpr typename auto& getDepthStencil();
	
	FramebufferObject MakeResized(GLuint width, GLuint height) const;

	template<int idx> FramebufferObject& operator<< (const detail::ClearF<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorF<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorI<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorU<idx>& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthF& cleardata);
	FramebufferObject& operator<< (const detail::ClearStencilI& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthStencilIF& cleardata);

	using FramebufferBase::operator<<;

};

template<typename InternalFormat_> auto MakeFramebuffer(Texture2D<InternalFormat_>&& tex);
template<typename InternalFormat_> auto MakeFramebuffer(const Texture2D<InternalFormat_>& tex);
template<typename InternalFormat_> auto MakeFramebuffer(Renderbuffer<InternalFormat_>&& ren);
template<class Atta, class ...As> auto MakeFramebuffer(Atta&& first_, As&&...tail_);
template<typename ...Attachments> using MakeFramebuffer_Type = decltype(MakeFramebuffer(Attachments(0,0)...));

} // namespace df

#include "Framebuffer.inl"
