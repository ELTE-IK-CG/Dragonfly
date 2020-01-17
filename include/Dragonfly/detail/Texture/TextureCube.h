#pragma once
#include "../../config.h"
#include "Texture.h"
#include <SDL/SDL_image.h>

namespace df
{

	namespace detail
	{
		constexpr bool IsTextureTypeCubeSide(TextureType type)
		{
			return (type == TextureType::TEX_CUBE_X_POS || type == TextureType::TEX_CUBE_X_NEG || type == TextureType::TEX_CUBE_Y_POS ||
				type == TextureType::TEX_CUBE_Y_NEG || type == TextureType::TEX_CUBE_Z_POS || type == TextureType::TEX_CUBE_Z_NEG);
		}
	}

enum TextureCubeFace{	X_POS = TextureType::TEX_CUBE_X_POS,
						X_NEG = TextureType::TEX_CUBE_X_NEG,
						Y_POS = TextureType::TEX_CUBE_Y_POS,
						Y_NEG = TextureType::TEX_CUBE_Y_NEG,
						Z_POS = TextureType::TEX_CUBE_Z_POS,
						Z_NEG = TextureType::TEX_CUBE_Z_NEG};

template<typename InternalFormat_>
class Texture<TextureType::TEX_CUBE_MAP, InternalFormat_> : public TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat_>
{
	using Base = TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat_>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

	void LoadFromSDLSurface(SDL_Surface* img, TextureType side);

	template<typename NewInternalFormat = InternalFormat_>
	Texture<TextureType::TEX_2D, NewInternalFormat> MakeFaceView(TextureCubeFace face, TexLevels levels = 0_levelAll);

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
	
	template<TextureType NewTexType = TextureType::TEX_CUBE_MAP, typename NewInternalFormat = InternalFormat_>
	auto MakeView(TexLevels levels = 0_levelAll);

	Texture<TextureType::TEX_2D, InternalFormat_> operator[](TextureCubeFace face);

	Texture operator[](TexLevels levels);
};

// TextureCube

template<typename InternalFormat_>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::LoadFromSDLSurface(SDL_Surface* img, TextureType side)
{
	ASSERT(detail::IsTextureTypeCubeSide(side), "TextureCube: side must be one of TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG}");
	GLenum target = static_cast<GLenum>(side);

	GLenum sdl_pxformat = GL_UNSIGNED_BYTE; //todo calculate from format
	Uint32 sdl_format = img->format->BytesPerPixel == 3 ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;
	GLenum sdl_channels = img->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
	if (img->format->format != sdl_format) {
		SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(img, sdl_format, 0);
		SDL_FreeSurface(img);
		ASSERT(formattedSurf != nullptr, "TextureCube: Failed to convert texture format.");
		img = formattedSurf;
	}

	glTexSubImage2D(target, 0, 0, 0, this->_width, this->_height, sdl_channels, sdl_pxformat, static_cast<void*>(img->pixels));

	SDL_FreeSurface(img);
}

template<typename InternalFormat_>
template<typename NewInternalFormat>
Texture<TextureType::TEX_2D, NewInternalFormat> Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::MakeFaceView(TextureCubeFace face, TexLevels levels)
{
	TexLayers layers{ static_cast<GLuint>(face) - static_cast<GLuint>(TextureType::TEX_CUBE_X_POS), 1 };
	if (levels.num == ALL) levels.num = this->_levels - levels.min;
	ASSERT(levels.min < this->_levels, "TextureCube: Too large mipmap index.");
	WARNING(levels.min + levels.num > this->_levels, "TextureCube: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	auto tex = this->_MakeView<TextureType::TEX_2D, NewInternalFormat>(levels, layers);
	tex.invertYOnFileLoad = false;
	return tex;
}

template<typename InternalFormat_>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::Texture(GLint size, GLint numLevels)
{
	InitTexture(size, numLevels);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::Texture(const std::string& Xpos, const std::string& Xneg, const std::string& Ypos, const std::string& Yneg, const std::string& Zpos, const std::string& Zneg)
{
	SDL_Surface* loaded_img = IMG_Load(Xpos.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Xpos + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h, "TextureCube: wrong image size");

	InitTexture(loaded_img->w, ALL);

	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_X_POS);

	loaded_img = IMG_Load(Xneg.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Xneg + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h && this->_width == loaded_img->w, "TextureCube: wrong image size");
	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_X_NEG);

	loaded_img = IMG_Load(Ypos.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Ypos + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h && this->_width == loaded_img->w, "TextureCube: wrong image size");
	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_Y_POS);

	loaded_img = IMG_Load(Yneg.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Yneg + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h && this->_width == loaded_img->w, "TextureCube: wrong image size");
	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_Y_NEG);

	loaded_img = IMG_Load(Zpos.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Zpos + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h && this->_width == loaded_img->w, "TextureCube: wrong image size");
	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_Z_POS);

	loaded_img = IMG_Load(Zneg.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load texture from \"" + Zneg + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h && this->_width == loaded_img->w, "TextureCube: wrong image size");
	LoadFromSDLSurface(loaded_img, TextureType::TEX_CUBE_Z_NEG);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>& Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat_>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::InitTexture(GLuint size, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "TextureCube: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = size;
		this->_height = size;
		this->_depth = 1;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(size))) + 1;
		this->_levels = numLevels;
		this->_layers = 6;
		this->bind();
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat_>();
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, iFormat, size, size);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat_>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::InitSizeFromFile(const std::string& file, GLuint numLevels)
{
	SDL_Surface* loaded_img = IMG_Load(file.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load file \"" + file + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h, "TextureCube: wrong image size");

	InitTexture(loaded_img->w, numLevels);

	SDL_FreeSurface(loaded_img);
}

template<typename InternalFormat_>
template<TextureType NewTexType, typename NewInternalFormat>
auto Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::MakeView(TexLevels levels)
{
	static_assert(NewTexType != TextureType::TEX_2D, "TextureCube: Use the TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG} instead of TextureType::TEX_2D.");
	static_assert(/*NewTexType == TextureType::TEX_2D || */    NewTexType == TextureType::TEX_2D_ARRAY
		|| NewTexType == TextureType::TEX_CUBE_MAP || NewTexType == TextureType::TEX_CUBE_MAP_ARRAY
		|| detail::IsTextureTypeCubeSide(NewTexType), "TextureCube: Incompatible view target.");
	if constexpr (detail::IsTextureTypeCubeSide(NewTexType))
	{
		return MakeFaceView<NewInternalFormat>(NewTexType, levels);
	}
	else {
		TexLayers layers = 0_layer >> 6;
		if (levels.num == ALL) levels.num = this->_levels - levels.min;
		ASSERT(levels.min < this->_levels, "TextureCube: Too large mipmap index.");
		WARNING(levels.min + levels.num > this->_levels, "TextureCube: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
		return this->_MakeView<NewTexType, NewInternalFormat>(levels, layers);
	}
}

template<typename InternalFormat_>
Texture<TextureType::TEX_2D, InternalFormat_> Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::operator[](TextureCubeFace face)
{
	ASSERT(detail::IsTextureTypeCubeSide(static_cast<TextureType>(face)), "TextureCube: face parameter has to be one of TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG}");
	return MakeFaceView(face);
}

template<typename InternalFormat_>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat_> Texture<TextureType::TEX_CUBE_MAP, InternalFormat_>::operator[](TexLevels levels)
{
	return MakeView(levels);
}

} //namespace df
