#pragma once
#include <string>
#include <vector>
#include <SDL/SDL_image.h>
#include "../../config.h"
#include "Texture.h"

namespace df
{
namespace detail
{
	int invert_image(int pitch, int height, void* image_pixels);

	template<unsigned depth_, unsigned stencil_, unsigned depthstencil_> struct FBO_compile_data;
}

template<typename InternalFormat_>
class Texture<TextureType::TEX_2D, InternalFormat_> : public TextureBase<TextureType::TEX_2D, InternalFormat_>
{
	using Base = TextureBase<TextureType::TEX_2D, InternalFormat_>;

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
	template<typename Format> Texture& operator= (const std::vector<Format>& data);

	void InitTexture(GLuint width, GLuint height, GLuint numLevels = ALL);

	Texture& LoadFromFile(const std::string& file);
	template<typename Format> Texture& LoadData(const std::vector<Format>& data, bool genMipmap = true);

	template<TextureType NewTexType = TextureType::TEX_2D, typename NewInternalFormat = InternalFormat_>
	Texture<NewTexType, NewInternalFormat> MakeView(TexLevels levels = 0_levelAll) const;

	Texture operator[] (TexLevels levels);

	Texture MakeResized(GLuint width, GLuint height) const;
};

template<typename InternalFormat_>
void Texture<TextureType::TEX_2D, InternalFormat_>::LoadFromSDLSurface(SDL_Surface* img)
{
	GLenum sdl_pxformat = GL_UNSIGNED_BYTE; //todo calculate from format
	Uint32 sdl_format = img->format->BytesPerPixel == 3 ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;
	GLenum sdl_channels = img->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
	if (img->format->format != sdl_format) {
		SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(img, sdl_format, 0);
		SDL_FreeSurface(img);
		ASSERT(formattedSurf != nullptr, "Texture2D: Failed to convert texture format.");
		img = formattedSurf;
	}

	if (invertYOnFileLoad) {
		int ret = detail::invert_image(img->pitch, img->h, img->pixels);
		ASSERT(ret == 0, "Texture2D: Failed to invert image");
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->_width, this->_height, sdl_channels, sdl_pxformat, static_cast<void*>(img->pixels));

	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(img);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_>::Texture(GLuint width, GLuint height, GLuint numLevels, bool invertImage)
	: invertYOnFileLoad(invertImage)
{
	InitTexture(width, height, numLevels);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_>::Texture(const std::string& file, GLuint numLevels, bool invertImage)
	: invertYOnFileLoad(invertImage)
{
	SDL_Surface* loaded_img = IMG_Load(file.c_str());
	ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());

	InitTexture(loaded_img->w, loaded_img->h, numLevels);

	LoadFromSDLSurface(loaded_img);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_>& Texture<TextureType::TEX_2D, InternalFormat_>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_>& Texture<TextureType::TEX_2D, InternalFormat_>::operator= (const std::string& file)
{
	return LoadFromFile(file);
}

template<typename InternalFormat_>
template<typename Format>
Texture<TextureType::TEX_2D, InternalFormat_>& Texture<TextureType::TEX_2D, InternalFormat_>::operator= (const std::vector<Format>& data)
{
	return LoadData(data);
}

template<typename InternalFormat_>
void Texture<TextureType::TEX_2D, InternalFormat_>::InitTexture(GLuint width, GLuint height, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "Texture2D: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = width;
		this->_height = height;
		this->_depth = 1;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(width > height ? width : height))) + 1;
		this->_levels = numLevels;
		this->_layers = 1;
		ASSERT(width >= 1 && height >= 1 && numLevels >= 1 && numLevels <= log2(width > height ? width : height) + 1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat_>();
		glTexStorage2D(GL_TEXTURE_2D, numLevels, iFormat, width, height);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_>& Texture<TextureType::TEX_2D, InternalFormat_>::LoadFromFile(const std::string& file)
{
	SDL_Surface* loaded_img = IMG_Load(file.c_str());
	ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());

	if (!this->_hasStorage) {
		InitTexture(loaded_img->w, loaded_img->h, ALL);
	}
	else {
		ASSERT(this->_width == loaded_img->w && this->_height == loaded_img->h, "Texture2D: cannot change texture's size after the storage has been set");
		this->bind();
	}
	LoadFromSDLSurface(loaded_img);

	return *this;
}

template<typename InternalFormat_>
template<typename Format>
Texture<TextureType::TEX_2D, InternalFormat_>& Texture<TextureType::TEX_2D, InternalFormat_>::LoadData(const std::vector<Format>& data, bool genMipmap)
{
	ASSERT(this->_hasStorage, "Texture2D: you can only load data to a texture if it has its size set (Init)");
	if (this->_hasStorage) {
		this->bind();

		constexpr GLenum pxFormat = detail::getInternalChannel<Format>();
		constexpr GLenum pxType   = detail::getInternalBaseType<Format>();

		ASSERT(this->_width * this->_height == data.size(), "Texture2D: trying to write a texture with the wrong amount of data");
		if (this->_width * this->_height > data.size())
			return *this;

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->_width, this->_height, pxFormat, pxType, static_cast<const void*>(&data[0]));

		if (genMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	return *this;
}

template<typename InternalFormat_>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> Texture<TextureType::TEX_2D, InternalFormat_>::MakeView(TexLevels levels) const
{
	static_assert(NewTexType == TextureType::TEX_2D || NewTexType == TextureType::TEX_2D_ARRAY, "Texture2D: Incompatible view target.");
	if (levels.num == ALL) levels.num = this->_levels - levels.min;
	ASSERT(levels.min < this->_levels, "Texture2D: Too large mipmap index.");
	WARNING(levels.min + levels.num > this->_levels, "Texture2D: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	return this->_MakeView<NewTexType, NewInternalFormat>(levels, 0_layer);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_> Texture<TextureType::TEX_2D, InternalFormat_>::operator[] (TexLevels levels)
{
	return MakeView(levels);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_> Texture<TextureType::TEX_2D, InternalFormat_>::MakeResized(GLuint width, GLuint height) const
{
	return Texture(width, height, this->_levels, this->invertYOnFileLoad);
}

} //namespace df
