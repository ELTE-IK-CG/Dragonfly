#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>
#include "../../config.h"
#include "../Traits/InternalFormats.h"

namespace df
{

enum class TextureType : decltype(GL_TEXTURE_1D) {
	TEX_1D = GL_TEXTURE_1D,					TEX_2D = GL_TEXTURE_2D,					TEX_3D = GL_TEXTURE_3D,
	TEX_RECTANGLE = GL_TEXTURE_RECTANGLE,	TEX_BUFFER = GL_TEXTURE_BUFFER,			TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	TEX_1D_ARRAY = GL_TEXTURE_1D_ARRAY,		TEX_2D_ARRAY = GL_TEXTURE_2D_ARRAY,		TEX_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
	TEX_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,		TEX_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
	TEX_CUBE_X_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_X,	TEX_CUBE_X_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	TEX_CUBE_Y_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	TEX_CUBE_Y_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	TEX_CUBE_Z_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	TEX_CUBE_Z_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

// TODO namespace
static const GLuint ALL = -1; // Mipmap level / array layer

struct TexLevelsAndLayers;

// Texture Mipmap levels
struct TexLevels {
	GLuint min;
	GLuint num;

	constexpr TexLevels operator>> (GLuint numLelvels) { return TexLevels{ this->min, numLelvels }; }
	constexpr operator TexLevelsAndLayers();
};
constexpr TexLevels operator"" _level(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), 1 }; }
constexpr TexLevels operator"" _levelAll(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), ALL }; }

// Texture Array layers
struct TexLayers {
	GLuint min;
	GLuint num;

	constexpr TexLayers operator>> (GLuint numLayers) { return TexLayers{ this->min, numLayers }; }
	constexpr operator TexLevelsAndLayers();
};
constexpr TexLayers operator"" _layer(unsigned long long int minLevel) { return TexLayers{ static_cast<GLuint>(minLevel), 1 }; }
constexpr TexLayers operator"" _layerAll(unsigned long long int minLevel) { return TexLayers{ static_cast<GLuint>(minLevel), ALL }; }

struct TexLevelsAndLayers {
	TexLevels levels;
	TexLayers layers;
};
constexpr TexLevelsAndLayers operator& (TexLevels levels, TexLayers layers) { return { levels, layers }; }
constexpr TexLevelsAndLayers operator& (TexLayers layers, TexLevels levels) { return { levels, layers }; }

inline constexpr TexLevels::operator TexLevelsAndLayers() { return { *this, {0, ALL} }; }
inline constexpr TexLayers::operator TexLevelsAndLayers() { return { {0, ALL}, *this }; }


class TextureLowLevelBase {
public:
	// raw OpenGL name
	explicit operator GLuint() const{ return texture_id; }
	// texture size
	GLuint getWidth() const { return _width; }
	GLuint getHeight() const { return _height; }
	GLuint getDepth() const { return _depth; }
	// mipmap levels
	GLuint getLevels() const { return _levels; }
	// array layers
	GLuint getLayers() const { return _layers; }
protected:
	GLuint texture_id = 0;
	GLuint _width = 0, _height = 0, _depth = 0;
	GLuint _levels = 0; // mipmap
	GLuint _layers = 0; // array
	bool _hasStorage = false;

	TextureLowLevelBase() { glGenTextures(1, &texture_id); }
	~TextureLowLevelBase() { glDeleteTextures(1, &texture_id); }

	TextureLowLevelBase(const TextureLowLevelBase&) = delete;
	TextureLowLevelBase(TextureLowLevelBase&& _o)
		: texture_id(_o.texture_id), _width(_o._width), _height(_o._height), _depth(_o._depth), _levels(_o._levels), _layers(_o._layers), _hasStorage(_o._hasStorage)
	{ _o.texture_id = 0; }

	TextureLowLevelBase& operator= (const TextureLowLevelBase&) = delete;
	TextureLowLevelBase& operator= (TextureLowLevelBase&& _o);
};

// **** Main Texture class ****

template<TextureType TexType, typename InternalFormat_>
class Texture{
	//Only specializations are allowed
	Texture() = delete;
};

template<TextureType TexType, typename InternalFormat_>
class TextureBase : public TextureLowLevelBase {
protected:
	TextureBase() {}
	~TextureBase() {}

	TextureBase(const TextureBase&) = delete;
	TextureBase(TextureBase&& _o) : TextureLowLevelBase(std::move(_o)) {}

	TextureBase& operator= (const TextureBase&) = delete;
	TextureBase& operator= (TextureBase&& _o);

	template<TextureType NewTexType, typename NewInternalFormat>
	Texture<NewTexType, NewInternalFormat> _MakeView(TexLevels levels, TexLayers layers) const;

public:
	using Internal_Format = InternalFormat_;

	void bind() const;
	void bind(GLuint hwSamplerUnit) const;
};


//implemented Texture 'instanses':

template<typename InternalFormat_ = glm::u8vec3>
using Texture2D = Texture<TextureType::TEX_2D, InternalFormat_>;

template<typename InternalFormat_ = glm::u8vec3>
using Texture2DArray = Texture<TextureType::TEX_2D_ARRAY, InternalFormat_>;

template<typename InternalFormat_> //no default, choose carefully!
using Texture3D = Texture<TextureType::TEX_3D, InternalFormat_>;

template<typename InternalFormat_ = glm::u8vec3>
using TextureCube = Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>;


// **** Implementation ****

template<TextureType TexType, typename InternalFormat_>
TextureBase<TexType, InternalFormat_>& TextureBase<TexType, InternalFormat_>::operator= (TextureBase&& _o) {
	TextureLowLevelBase::operator=(std::move(_o));
	return *this;
}

template<TextureType TexType, typename InternalFormat_>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> TextureBase<TexType, InternalFormat_>::_MakeView(TexLevels levels, TexLayers layers) const
{
	ASSERT(this->_hasStorage, "Texture: Cannot create a view from a texture that has no storage.");
	ASSERT(levels.min < this->_levels && levels.min + levels.num <= this->_levels, "Texture: wrong mipmap level arguments");
	ASSERT(layers.min < this->_layers && layers.min + layers.num <= this->_layers, "Texture: wrong array layer arguments");
	Texture<NewTexType, NewInternalFormat> view;
	if (this->_hasStorage) {
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat_>();
		static_assert(sizeof(InternalFormat_) == sizeof(NewInternalFormat), "Texture: Internal formats must be of the same size class");
		glTextureView(view.texture_id, static_cast<GLenum>(NewTexType), this->texture_id, iFormat, levels.min, levels.num, layers.min, layers.num);
		GL_CHECK;
		view._width = std::max((GLuint)1, this->_width >> levels.min);
		view._height = std::max((GLuint)1, this->_height >> levels.min);
		view._depth = std::max((GLuint)1, this->_depth >> levels.min);
		view._levels = levels.num;
		view._layers = layers.num;
		view._hasStorage = true;
	}
	return std::move(view);
}

template<TextureType TexType, typename InternalFormat_>
void TextureBase<TexType, InternalFormat_>::bind() const {
	glBindTexture(static_cast<GLenum>(TexType), this->texture_id);
}

template<TextureType TexType, typename InternalFormat_>
void TextureBase<TexType, InternalFormat_>::bind(GLuint hwSamplerUnit) const {
	ASSERT(hwSamplerUnit < 256, "Texture or sampler units you can attach your texture to start from 0 (and go to 96 minimum in OpenGL 4.5.)");
	glActiveTexture(GL_TEXTURE0 + hwSamplerUnit);
	this->bind();
}

inline TextureLowLevelBase& df::TextureLowLevelBase::operator=(TextureLowLevelBase&& _o) {
	std::swap(texture_id, _o.texture_id);
	_width = _o._width;
	_height = _o._height;
	_depth = _o._depth;
	_levels = _o._levels;
	_layers = _o._layers;
	_hasStorage = _o._hasStorage;
	return *this;
}

} //namespace df