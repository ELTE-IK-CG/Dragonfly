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
		template<typename F> static constexpr void static_addition_check() {
			static_assert(depth() == -1        || !has_depth_v<F>   && !has_depthstencil_v<F>, "An FBO cannot have two depth textures.");
			static_assert(stencil() == -1      || !has_stencil_v<F> && !has_depthstencil_v<F>, "An FBO cannot have two stencil textures.");
			static_assert(depthstencil() == -1 || !has_depthstencil_v<F>, "An FBO cannot have two depth-stencil textures, that is just too much!");
		}
		template<typename F, unsigned size> using _add_InternalFormat_t = FBO_compile_data<has_depth_v<F> ? size : -1, has_stencil_v<F> ? size : -1, has_depthstencil_v<F> ? size : -1>;
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

public:
	FramebufferObject(GLuint id, std::tuple<Attachements...> &&attachements, GLuint width = 0, GLuint height = 0) : FramebufferBase(id, 0,0,width,height), _attachements(std::move(attachements)), _width(width), _height(height){}
	FramebufferObject() { glCreateFramebuffers(1, &_id); }
	~FramebufferObject() { if(_id != 0) glDeleteFramebuffers(1, &_id); }
	FramebufferObject(FramebufferObject&&) = default;

	template<typename InternalFormat> FramebufferObject(Texture2D<InternalFormat>&& tex) ;
	template<typename InternalFormat> FramebufferObject(const Texture2D<InternalFormat>& tex) : FramebufferObject(tex.MakeView(0_level)) {}
	template<typename InternalFormat> FramebufferObject(Renderbuffer<InternalFormat>&& ren);
	template<typename InternalFormat> FramebufferObject(const Renderbuffer<InternalFormat>& ren) = delete;
	//FramebufferObject& operator=(FramebufferObject&&) = default;
	
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (Texture2D<InternalFormat>&& tex) &&;
	template<typename InternalFormat> _add_Texture2D_t<InternalFormat> operator + (const Texture2D<InternalFormat> &tex) &&	{ return std::move(*this) + tex.MakeView(0_level);}

	template<typename InternalFormat> _add_Renderbuffer_t<InternalFormat> operator + (Renderbuffer<InternalFormat>&& ren) &&;
	template<typename InternalFormat> _add_Renderbuffer_t<InternalFormat> operator + (const Renderbuffer<InternalFormat> &ren) && = delete;

	template<int idx> constexpr typename auto& get();

	template<int idx> FramebufferObject& operator<< (const detail::ClearColorF<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorI<idx>& cleardata);
	template<int idx> FramebufferObject& operator<< (const detail::ClearColorU<idx>& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthF& cleardata);
	FramebufferObject& operator<< (const detail::ClearStencilI& cleardata);
	FramebufferObject& operator<< (const detail::ClearDepthStencilIF& cleardata);

	using FramebufferBase::operator<<;
};

using EmptyFBO = FramebufferObject<detail::FBO_compile_data<>>;

template<typename ... ColorAttachements>
using FBO = FramebufferObject<Renderbuffer<depth24>, ColorAttachements...>;

template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::FramebufferObject(Texture2D<InternalFormat>&& tex)
	: FramebufferBase(0, 0, 0, tex.getWidth(), tex.getHeight()),
	_attachements(std::make_tuple(std::move(tex))), _width(tex.getWidth()), _height(tex.getHeight())
{
	glCreateFramebuffers(1, &this->_id);
	this->bind();
	detail::attach2BoundFbo<0>(tex);
}

template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::FramebufferObject(Renderbuffer<InternalFormat>&& ren)
	: FramebufferBase(0, 0, 0, ren.getWidth(), ren.getHeight()),
	_attachements(std::make_tuple(std::move(ren))), _width(ren.getWidth()), _height(ren.getHeight())
{
	glCreateFramebuffers(1, &this->_id);
	this->bind();
	detail::attach2BoundFbo<0>(ren);
}

//tex -> fbo
template<typename InternalFormat>
auto MakeFramebuffer(df::Texture2D<InternalFormat>&& tex) {
	return FramebufferObject<typename detail::FBO_compile_data<>::template _add_InternalFormat_t<InternalFormat, 0>, Texture2D<InternalFormat>>(std::move(tex));
}
template<typename InternalFormat>
auto MakeFramebuffer(const df::Texture2D<InternalFormat>& tex) {
	return FramebufferObject<typename detail::FBO_compile_data<>::template _add_InternalFormat_t<InternalFormat, 0>, Texture2D<InternalFormat>>(tex.MakeView(0_level));
}
//ren -> fbo
template<typename InternalFormat>
auto MakeFramebuffer(df::Renderbuffer<InternalFormat>&& ren) {
	return FramebufferObject<typename detail::FBO_compile_data<>::template _add_InternalFormat_t<InternalFormat, 0>, Renderbuffer<InternalFormat>>(std::move(ren));
}

// tex + tex (for convenience)
template<typename F1, typename F2> auto operator+ (const Texture2D<F1>&  tex1, const Texture2D<F2>&  tex2) { return MakeFramebuffer(tex1)            + tex2; }
template<typename F1, typename F2> auto operator+ (      Texture2D<F1>&& tex1, const Texture2D<F2>&  tex2) { return MakeFramebuffer(std::move(tex1)) + tex2; }
template<typename F1, typename F2> auto operator+ (const Texture2D<F1>&  tex1,       Texture2D<F2>&& tex2) { return MakeFramebuffer(tex1)            + std::move(tex2); }
template<typename F1, typename F2> auto operator+ (      Texture2D<F1>&& tex1,       Texture2D<F2>&& tex2) { return MakeFramebuffer(std::move(tex1)) + std::move(tex2); }
// tex + ren
template<typename F1, typename F2> auto operator+ (const Texture2D<F1>&  tex1,    Renderbuffer<F2>&& ren2) { return MakeFramebuffer(tex1)            + std::move(ren2); }
template<typename F1, typename F2> auto operator+ (      Texture2D<F1>&& tex1,    Renderbuffer<F2>&& ren2) { return MakeFramebuffer(std::move(tex1)) + std::move(ren2); }
// ren + tex
template<typename F1, typename F2> auto operator+ (  Renderbuffer<F1>&&  ren1, const Texture2D<F2>&  tex2) { return MakeFramebuffer(std::move(ren1)) + tex2; }
template<typename F1, typename F2> auto operator+ (  Renderbuffer<F1>&&  ren1,       Texture2D<F2>&& tex2) { return MakeFramebuffer(std::move(ren1)) + std::move(tex2); }
// ren + ren
template<typename F1, typename F2> auto operator+ (  Renderbuffer<F1>&&  ren1,    Renderbuffer<F2>&& ren2) { return MakeFramebuffer(std::move(ren1)) + std::move(ren2); }
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

//fbo + tex
template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::_add_Texture2D_t<InternalFormat> FramebufferObject<compile_data, Attachements...>::operator+(Texture2D<InternalFormat>&& tex) &&
{
	compile_data::template static_addition_check<InternalFormat>();

	constexpr int idx = sizeof...(Attachements);
	constexpr int index = idx - calc_extra_spots_until<idx>();
	static_assert(index >= 0 && index <= sizeof...(Attachements), "Index out of bounds here.");

	ASSERT((this->_width == 0 || tex.getWidth() == 0 || this->_width == tex.getWidth()) && (this->_height == 0 || tex.getHeight() == 0 || this->_height == tex.getHeight()), "Unmaching texture size in framebuffer object.");
	int w = (this->_width == 0 ? tex.getWidth() : this->_width), h = (this->_height == 0 ? tex.getHeight() : this->_height);

	this->bind();
	detail::attach2BoundFbo<index>(tex);

	GLuint name = this->_id;
	this->_id = 0;
	return _add_Texture2D_t<InternalFormat>(name, std::tuple_cat(std::move(this->_attachements), std::make_tuple(std::move(tex))), w, h);
}

//fbo + ren
template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::_add_Renderbuffer_t<InternalFormat> FramebufferObject<compile_data, Attachements...>::operator+(Renderbuffer<InternalFormat>&& ren) &&
{
	compile_data::template static_addition_check<InternalFormat>();

	constexpr int idx = sizeof...(Attachements);
	constexpr int index = idx - calc_extra_spots_until<idx>();
	static_assert(index >= 0 && index <= sizeof...(Attachements), "Index out of bounds here.");

	ASSERT((this->_width == 0 || ren.getWidth() == 0 || this->_width == ren.getWidth()) && (this->_height == 0 || ren.getHeight() == 0 || this->_height == ren.getHeight()), "Unmaching renderbuffer size in framebuffer object.");
	int w = (this->_width == 0 ? ren.getWidth() : this->_width), h = (this->_height == 0 ? ren.getHeight() : this->_height);

	this->bind();
	detail::attach2BoundFbo<index>(ren);

	GLuint name = this->_id;
	this->_id = 0;
	return _add_Renderbuffer_t<InternalFormat>(name, std::tuple_cat(std::move(this->_attachements), std::make_tuple(std::move(ren))), w, h);
}

//indexing
template<typename compile_data, typename ...Attachements> template<int idx>
constexpr int FramebufferObject<compile_data, Attachements...>::calc_extra_spots_until()
{
	static_assert(idx >= 0 && idx <= sizeof...(Attachements), "Index out of bounds here.");
	return (idx > compile_data::depth() ? 1 : 0) + (idx > compile_data::stencil() ? 1 : 0) + (idx > compile_data::depthstencil() ? 1 : 0);
}

//<<Clean
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorF<idx>& cleardata)
{	//TODO: better type check
	static_assert(!detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->get<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with floats if it contains integral values.");
	this->bind();
	glClearBufferfv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorI<idx>& cleardata) {
	static_assert(detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->get<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with integers if it contains floating values.");
	this->bind();
	glClearBufferiv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorU<idx>& cleardata) {
	static_assert(detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->get<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with integers if it contains floating values.");
	this->bind();
	glClearBufferuv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& df::FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearDepthF& cleardata) {
	this->bind();
	glClearBufferfv(GL_DEPTH, 0, &cleardata._depth);	//check?
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& df::FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearStencilI& cleardata) {
	this->bind();
	glClearBufferiv(GL_DEPTH, 0, &cleardata._stencil);
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& df::FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearDepthStencilIF& cleardata) {
	this->bind();
	glClearBufferfi(GL_DEPTH_STENCIL, 0, cleardata._depth, cleardata._stencil);
	return *this;
}

template<int index, typename InternalFormat>
void detail::attach2BoundFbo(const Texture2D<InternalFormat>& tex)
{
	constexpr bool is_color = detail::is_color_attachement_v<InternalFormat>;
	constexpr GLenum attachement = is_color ? GL_COLOR_ATTACHMENT0 + index : detail::get_attachement_v<InternalFormat>;
	if constexpr (is_color) {
		GLenum buffs[index + 1];
		for (int i = 0; i <= index; ++i) buffs[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(1, buffs);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachement, GL_TEXTURE_2D, (GLuint)tex, 0);
	std::cout << "Texture2D " << (detail::is_color_attachement_v<InternalFormat> ? "COLOR_ATTACHEMENT_ " : "DEPTH_") <<
		(detail::is_color_attachement_v<InternalFormat> ? index : 0) << "w = " << tex.getWidth() << " h = " << tex.getHeight() << std::endl;
}

template<int index, typename InternalFormat>
void detail::attach2BoundFbo(const Renderbuffer<InternalFormat>& ren)
{
	constexpr bool is_color = detail::is_color_attachement_v<InternalFormat>;
	constexpr GLenum attachement =  is_color ? GL_COLOR_ATTACHMENT0 + index : detail::get_attachement_v<InternalFormat>;
	if constexpr (is_color) {
		GLenum buffs[index+1];
		for (int i = 0; i <= index; ++i) buffs[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(1, buffs);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachement, GL_RENDERBUFFER, (GLuint)ren);
	std::cout << "Renderbuffer " << (detail::is_color_attachement_v<InternalFormat> ? "COLOR_ATTACHEMENT_ " : "DEPTH_") <<
		(detail::is_color_attachement_v<InternalFormat> ? index : 0) << "w = " << ren.getWidth() << " h = " << ren.getHeight() << std::endl;
}

template<typename compile_data, typename ...Attachements>
template<int idx> constexpr auto& df::FramebufferObject<compile_data, Attachements...>::get()
{
	constexpr int index = idx + calc_extra_spots_until<idx>();
	static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
	return std::get< index >(_attachements);
}

} // namespace df

