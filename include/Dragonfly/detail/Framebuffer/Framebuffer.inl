#pragma once
#include "../../config.h"
#include "../Texture/Texture2D.h"
#include "../Framebuffer/Framebuffer.h"
#include <iostream>

namespace df
{

template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::FramebufferObject(Texture2D<InternalFormat>&& tex)
	: FramebufferBase(0, 0, 0, tex.getWidth(), tex.getHeight()),
	_attachements(std::make_tuple(std::move(tex))), _width(tex.getWidth()), _height(tex.getHeight())
{
	glCreateFramebuffers(1, &this->_id);
	this->bind();
	detail::attach2BoundFbo<0>(std::get<0>(_attachements));
}

template<typename compile_data, typename ...Attachements> template<typename InternalFormat>
FramebufferObject<compile_data, Attachements...>::FramebufferObject(Renderbuffer<InternalFormat>&& ren)
	: FramebufferBase(0, 0, 0, ren.getWidth(), ren.getHeight()),
	_attachements(std::make_tuple(std::move(ren))), _width(ren.getWidth()), _height(ren.getHeight())
{
	glCreateFramebuffers(1, &this->_id);
	this->bind();
	detail::attach2BoundFbo<0>(std::get<0>(_attachements));
}

//tex -> fbo
template<typename InternalFormat>
auto MakeFramebuffer(Texture2D<InternalFormat>&& tex) {
	return FramebufferObject<typename detail::FBO_compile_data<>::template _add_InternalFormat_t<InternalFormat, 0>, Texture2D<InternalFormat>>(std::move(tex));
}
template<typename InternalFormat>
auto MakeFramebuffer(const Texture2D<InternalFormat>& tex) {
	return FramebufferObject<typename detail::FBO_compile_data<>::template _add_InternalFormat_t<InternalFormat, 0>, Texture2D<InternalFormat>>(tex.MakeView(0_level));
}
//ren -> fbo
template<typename InternalFormat>
auto MakeFramebuffer(Renderbuffer<InternalFormat>&& ren) {
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
	static_assert(idx >= -1 && idx <= sizeof...(Attachements), "Index out of bounds here.");
	return (idx > compile_data::depth() ? 1 : 0) + (idx > compile_data::stencil() ? 1 : 0) + (idx > compile_data::depthstencil() ? 1 : 0);
}

template<typename compile_data, typename ...Attachements> template<int idx>
constexpr void FramebufferObject<compile_data, Attachements...>::attach_all_upto_idx_to_bound_fb_with_given_size()
{
	attach_all_upto_idx_to_bound_fb_with_given_size<idx - 1>();
	auto& texren = std::get<idx>(this->_attachements);
	constexpr int index = idx - calc_extra_spots_until<idx>();
	static_assert(index >= 0 && index <= sizeof...(Attachements), "Index out of bounds here.");
	detail::attach2BoundFbo<index>(texren);
}

//<<Clean
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorF<idx>& cleardata)
{	//TODO: better type check
	static_assert(!detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->getColor<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with floats if it contains integral values.");
	this->bind();
	glClearBufferfv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorI<idx>& cleardata) {
	static_assert(detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->getColor<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with integers if it contains floating values.");
	this->bind();
	glClearBufferiv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearColorU<idx>& cleardata) {
	static_assert(detail::isInternalFormatIntegralType<typename std::remove_reference_t<decltype(this->getColor<idx>())>::PixelFormat>(), "Cannot clear an framebuffer color attachement with integers if it contains floating values.");
	this->bind();
	glClearBufferuv(GL_COLOR, idx, &cleardata._red);
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearDepthF& cleardata) {
	this->bind();
	glClearBufferfv(GL_DEPTH, 0, &cleardata._depth);	//check?
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearStencilI& cleardata) {
	this->bind();
	glClearBufferiv(GL_DEPTH, 0, &cleardata._stencil);
	return *this;
}
template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearDepthStencilIF& cleardata) {
	this->bind();
	glClearBufferfi(GL_DEPTH_STENCIL, 0, cleardata._depth, cleardata._stencil);
	return *this;
}
template<typename compile_data, typename ...Attachements> template<int idx>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator<<(const detail::ClearF<idx>& cleardata) {
	this->bind();
	glClearBufferfv(GL_COLOR, idx, &cleardata.color._red);
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
		glDrawBuffers(index + 1, buffs);
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
		glDrawBuffers(index + 1, buffs);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachement, GL_RENDERBUFFER, (GLuint)ren);
	std::cout << "Renderbuffer " << (detail::is_color_attachement_v<InternalFormat> ? "COLOR_ATTACHEMENT_ " : "DEPTH_") <<
		(detail::is_color_attachement_v<InternalFormat> ? index : 0) << "w = " << ren.getWidth() << " h = " << ren.getHeight() << std::endl;
}

template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...>& FramebufferObject<compile_data, Attachements...>::operator=(FramebufferObject<compile_data, Attachements...> && _o)
{
	_attachements = std::move(_o._attachements);
	auto thisId = this->_id;
	FramebufferBase::operator=(std::move(_o));
	_o._id = thisId;
	_width = _o._width;
	_height = _o._height;

	return *this;
}

template<typename compile_data, typename ...Attachements> template<int idx>
constexpr auto& FramebufferObject<compile_data, Attachements...>::getColor()
{
	constexpr int index = idx + calc_extra_spots_until<idx+1>();
	static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
	return std::get< index >(_attachements);
}

template<typename compile_data, typename ...Attachements>
constexpr auto& FramebufferObject<compile_data, Attachements...>::getDepth()
{
	static_assert(compile_data::depth() != detail::_none_, "This FramebufferObject does not have a depth-only attachement.");
	constexpr int index = compile_data::depth();
	static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
	return std::get< index >(_attachements);
}
template<typename compile_data, typename ...Attachements>
constexpr auto& FramebufferObject<compile_data, Attachements...>::getStencil()
{
	static_assert(compile_data::stencil() != detail::_none_, "This FramebufferObject does not have a stencil-only attachement.");
	constexpr int index = compile_data::stencil();
	static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
	return std::get< index >(_attachements);
}
template<typename compile_data, typename ...Attachements>
constexpr auto& FramebufferObject<compile_data, Attachements...>::getDepthStencil()
{
	static_assert(compile_data::depthstencil() != detail::_none_, "This FramebufferObject does not have a combined depth-stencil attachement.");
	constexpr int index = compile_data::depthstencil();
	static_assert(index >= 0 && index < sizeof...(Attachements), "Index out of bounds here.");
	return std::get< index >(_attachements);
}

template<typename compile_data, typename ...Attachements>
FramebufferObject<compile_data, Attachements...> FramebufferObject<compile_data, Attachements...>::MakeResized(GLuint width, GLuint height) const
{
	GLuint id; glCreateFramebuffers(1, &id);
	FramebufferObject<compile_data, Attachements...> ret(id, std::make_tuple(Attachements(width, height) ...), width, height);
	ret.bind();
	ret.attach_all_upto_idx_to_bound_fb_with_given_size<sizeof...(Attachements) - 1>();
	return ret;
}

} // namespace df

