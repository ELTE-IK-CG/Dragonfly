#pragma once
#include <string>
#include <ElteCG/define.h>
#include <ElteCG/ogl/texture_helper.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL/SDL_image.h>

enum class TextureType : decltype(GL_TEXTURE_1D)
{
	TEX_1D = GL_TEXTURE_1D,							TEX_2D = GL_TEXTURE_2D,					TEX_3D = GL_TEXTURE_3D,
	TEX_RECTANGLE = GL_TEXTURE_RECTANGLE,			TEX_BUFFER = GL_TEXTURE_BUFFER,			TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	TEX_1D_ARRAY = GL_TEXTURE_1D_ARRAY,				TEX_2D_ARRAY = GL_TEXTURE_2D_ARRAY,		TEX_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
	TEX_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,	TEX_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
	TEX_CUBE_X_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_X,	TEX_CUBE_X_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	TEX_CUBE_Y_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	TEX_CUBE_Y_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	TEX_CUBE_Z_POS = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	TEX_CUBE_Z_NEG = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

namespace detail
{
	constexpr bool IsTextureTypeCubeSide(TextureType type)
	{
		return (type == TextureType::TEX_CUBE_X_POS || type == TextureType::TEX_CUBE_X_NEG || type == TextureType::TEX_CUBE_Y_POS ||
			type == TextureType::TEX_CUBE_Y_NEG || type == TextureType::TEX_CUBE_Z_POS || type == TextureType::TEX_CUBE_Z_NEG);
	}

	int invert_image(int pitch, int height, void* image_pixels);
}

// TODO namespace
static const GLuint ALL = -1; // Mipmap level / array layer

// Texture Mipmap levels
struct TexLevels {
	GLuint min;
	GLuint num;

	constexpr TexLevels operator>> (GLuint numLelvels) { return TexLevels{ this->min, numLelvels }; }
};
constexpr TexLevels operator"" _level(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), 1 }; }
constexpr TexLevels operator"" _levelAll(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), ALL }; }

// Texture Array layers
struct TexLayers {
	GLuint min;
	GLuint num;

	constexpr TexLayers operator>> (GLuint numLayers) { return TexLayers{ this->min, numLayers }; }
};
constexpr TexLayers operator"" _layer(unsigned long long int minLevel) { return TexLayers{ static_cast<GLuint>(minLevel), 1 }; }
constexpr TexLayers operator"" _layerAll(unsigned long long int minLevel) { return TexLayers{ static_cast<GLuint>(minLevel), ALL }; }

struct TexLevelsAndLayers {
	TexLevels levels;
	TexLayers layers;
};
constexpr TexLevelsAndLayers operator& (TexLevels levels, TexLayers layers) { return { levels, layers }; }
constexpr TexLevelsAndLayers operator& (TexLayers layers, TexLevels levels) { return { levels, layers }; }


class TextureLowLevelBase {
public:
	operator GLuint() { return texture_id; }
protected:
	GLuint texture_id = 0;
	
	GLuint _width = 0, _height = 0;
	GLuint _levels = 0; // mipmap
	GLuint _layers = 0; // array
	bool _hasStorage = false;

	TextureLowLevelBase() { glGenTextures(1, &texture_id); }
	~TextureLowLevelBase() { glDeleteTextures(1, &texture_id); }

	TextureLowLevelBase(const TextureLowLevelBase&) = delete;
	TextureLowLevelBase(TextureLowLevelBase&& _o)
		: texture_id(_o.texture_id), _width(_o._width), _height(_o._height), _levels(_o._levels), _layers(_o._layers), _hasStorage(_o._hasStorage)
	{ _o.texture_id = 0; }

	TextureLowLevelBase& operator= (const TextureLowLevelBase&) = delete;
	TextureLowLevelBase& operator= (TextureLowLevelBase&& _o) {
		GLuint temp = texture_id;
		texture_id = _o.texture_id;
		_o.texture_id = temp;

		_width = _o._width;
		_height = _o._height;
		_levels = _o._levels;
		_layers = _o._layers;
		_hasStorage = _o._hasStorage;
		return *this;
	}
};

template<TextureType TexType, typename InternalFormat>
class Texture{
	//Only specializations are allowed
	Texture() = delete;
};

template<TextureType TexType, typename InternalFormat>
class TextureBase : public TextureLowLevelBase
{
protected:
	TextureBase() {}
	~TextureBase() {}

	TextureBase(const TextureBase&) = delete;
	TextureBase(TextureBase&& _o) : TextureLowLevelBase(std::move(_o)) {}

	TextureBase& operator= (const TextureBase&) = delete;
	TextureBase& operator= (TextureBase&& _o);

	template<TextureType NewTexType, typename NewInternalFormat>
	Texture<NewTexType, NewInternalFormat> _MakeView(TexLevels levels, TexLayers layers);

public:
	void bind() const;
	void bind(GLuint hwSamplerUnit) const;
};


template<typename InternalFormat>
class Texture<TextureType::TEX_2D, InternalFormat> : public TextureBase<TextureType::TEX_2D, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_2D, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

	void LoadFromSDLSurface(SDL_Surface* img);

public:
	bool invertYOnFileLoad = true;

	Texture() {}
	Texture(GLuint width, GLuint height, GLuint numLevels = ALL, bool invertImage = true);
	Texture(const std::string &file, GLuint numLevels = ALL, bool invertImage = true);
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o);

	Texture& operator= (const std::string& file);

	void InitTexture(GLuint width, GLuint height, GLuint numLevels = ALL);

	Texture& LoadFromFile(const std::string& file);

	template<TextureType NewTexType = TextureType::TEX_2D, typename NewInternalFormat = InternalFormat>
	Texture<NewTexType, NewInternalFormat> MakeView(TexLevels levels = 0_levelAll);

	Texture<TextureType::TEX_2D, InternalFormat> operator[] (TexLevels levels);
};

template<typename InternalFormat>
class Texture<TextureType::TEX_CUBE_MAP, InternalFormat> : public TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

	void LoadFromSDLSurface(SDL_Surface* img, TextureType side);

	template<typename NewInternalFormat = InternalFormat>
	Texture<TextureType::TEX_2D, NewInternalFormat> MakeFaceView(TextureType face, TexLevels levels = 0_levelAll);

public:
	Texture() {}
	Texture(GLint size, GLint numLevels = ALL);
	Texture(const std::string& Xpos, const std::string& Xneg, const std::string& Ypos, const std::string& Yneg, const std::string& Zpos, const std::string& Zneg);
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o);

	void InitTexture(GLuint size, GLuint numLevels = ALL);
	void InitSizeFromFile(const std::string& file, GLuint numLevels = ALL);
	
	template<TextureType NewTexType = TextureType::TEX_CUBE_MAP, typename NewInternalFormat = InternalFormat>
	auto MakeView(TexLevels levels = 0_levelAll);

	Texture<TextureType::TEX_2D, InternalFormat> operator[](TextureType face);

	Texture operator[](TexLevels levels);
};

template<typename InternalFormat = glm::u8vec3>
using Texture2D = Texture<TextureType::TEX_2D, InternalFormat>;
template<typename InternalFormat = glm::u8vec3>
using TextureCube = Texture<TextureType::TEX_CUBE_MAP, InternalFormat>;

#include "Texture.inl"
