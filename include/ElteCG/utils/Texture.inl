#pragma once
#include "Texture.h"

// TextureBase

template<TextureType TexType, typename InternalFormat>
TextureBase<TexType, InternalFormat>& TextureBase<TexType, InternalFormat>::operator= (TextureBase&& _o) {
	TextureLowLevelBase::operator=(std::move(_o));
	return *this;
}

template<TextureType TexType, typename InternalFormat>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> TextureBase<TexType, InternalFormat>::_MakeView(TexLevels levels, TexLayers layers)
{
	ASSERT(this->_hasStorage, "Texture: Cannot create a view from a texture that has no storage.");
	ASSERT(levels.min < this->_levels && levels.min + levels.num <= this->_levels, "Texture: wrong mipmap level arguments");
	ASSERT(layers.min < this->_layers && layers.min + layers.num <= this->_layers, "Texture: wrong array layer arguments");
	Texture<NewTexType, NewInternalFormat> view;
	if (this->_hasStorage) {
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		static_assert(sizeof(InternalFormat) == sizeof(NewInternalFormat), "Texture: Internal formats must be of the same size class");
		glTextureView(view.texture_id, static_cast<GLenum>(NewTexType), this->texture_id, iFormat, levels.min, levels.num, layers.min, layers.num);
		GL_CHECK;
		view._width = this->_width >> levels.min;
		view._height = this->_height >> levels.min;
		view._levels = levels.num;
		view._layers = layers.num;
		view._hasStorage = true;
	}
	return std::move(view);
}

template<TextureType TexType, typename InternalFormat>
void TextureBase<TexType, InternalFormat>::bind() const
{
	glBindTexture(static_cast<GLenum>(TexType), this->texture_id);
}

template<TextureType TexType, typename InternalFormat>
void TextureBase<TexType, InternalFormat>::bind(GLuint hwSamplerUnit) const {
	ASSERT(hwSamplerUnit < 256, "Texture or sampler units you can attach your texture to start from 0 (and go to 96 minimum in OpenGL 4.5.)");
	glActiveTexture(GL_TEXTURE0 + hwSamplerUnit);
	this->bind();
}

// Texture2D

template<typename InternalFormat>
void Texture<TextureType::TEX_2D, InternalFormat>::LoadFromSDLSurface(SDL_Surface* img)
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

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>::Texture(GLuint width, GLuint height, GLuint numLevels, bool invertImage)
	: invertYOnFileLoad(invertImage)
{
	InitTexture(width, height, numLevels);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>::Texture(const std::string &file, GLuint numLevels, bool invertImage)
	: invertYOnFileLoad(invertImage)
{
	SDL_Surface* loaded_img = IMG_Load(file.c_str());
	ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());

	InitTexture(loaded_img->w, loaded_img->h, numLevels);

	LoadFromSDLSurface(loaded_img);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>& Texture<TextureType::TEX_2D, InternalFormat>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>& Texture<TextureType::TEX_2D, InternalFormat>::operator= (const std::string& file)
{
	return LoadFromFile(file);
}

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_2D, InternalFormat>& Texture<TextureType::TEX_2D, InternalFormat>::operator= (const std::vector<Format>& data)
{
	return LoadData(data);
}

template<typename InternalFormat>
void Texture<TextureType::TEX_2D, InternalFormat>::InitTexture(GLuint width, GLuint height, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "Texture2D: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = width;
		this->_height = height;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(width > height ? width : height))) + 1;
		this->_levels = numLevels;
		this->_layers = 1;
		ASSERT(width >= 1 && height >= 1 && numLevels >= 1 && numLevels <= log2(width > height ? width : height) + 1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_2D, numLevels, iFormat, width, height);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat>& Texture<TextureType::TEX_2D, InternalFormat>::LoadFromFile(const std::string& file)
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

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_2D, InternalFormat>& Texture<TextureType::TEX_2D, InternalFormat>::LoadData(const std::vector<Format>& data, bool genMipmap)
{
	ASSERT(this->_hasStorage, "Texture2D: you can only load data to a texture if it has its size set (Init)");
	if (this->_hasStorage) {
		this->bind();

		constexpr GLenum pxFormat = eltecg::ogl::helper::getInternalChannel<Format>();
		constexpr GLenum pxType = eltecg::ogl::helper::getInternalBaseType<Format>();

		ASSERT(this->_width * this->_height == data.size(), "Texture2D: trying to write a texture with the wrong amount of data");
		if (this->_width * this->_height > data.size())
			return *this;

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->_width, this->_height, pxFormat, pxType, static_cast<const void*>(&data[0]));

		if(genMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	return *this;
}

template<typename InternalFormat>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> Texture<TextureType::TEX_2D, InternalFormat>::MakeView(TexLevels levels)
{
	static_assert(NewTexType == TextureType::TEX_2D || NewTexType == TextureType::TEX_2D_ARRAY, "Texture2D: Incompatible view target.");
	if (levels.num == ALL) levels.num = this->_levels - levels.min;
	ASSERT(levels.min < this->_levels, "Texture2D: Too large mipmap index.");
	WARNING(levels.min + levels.num > this->_levels, "Texture2D: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	return this->_MakeView<NewTexType, NewInternalFormat>(levels, 0_layer);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat> Texture<TextureType::TEX_2D, InternalFormat>::operator[] (TexLevels levels)
{
	return MakeView(levels);
}

// TextureCube

template<typename InternalFormat>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::LoadFromSDLSurface(SDL_Surface* img, TextureType side)
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

template<typename InternalFormat>
template<typename NewInternalFormat>
Texture<TextureType::TEX_2D, NewInternalFormat> Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::MakeFaceView(TextureType face, TexLevels levels)
{
	TexLayers layers{ static_cast<GLuint>(face) - static_cast<GLuint>(TextureType::TEX_CUBE_X_POS), 1 };
	if (levels.num == ALL) levels.num = this->_levels - levels.min;
	ASSERT(levels.min < this->_levels, "TextureCube: Too large mipmap index.");
	WARNING(levels.min + levels.num > this->_levels, "TextureCube: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	auto tex = this->_MakeView<TextureType::TEX_2D, NewInternalFormat>(levels, layers);
	tex.invertYOnFileLoad = false;
	return tex;
}

template<typename InternalFormat>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::Texture(GLint size, GLint numLevels)
{
	InitTexture(size, numLevels);
}

template<typename InternalFormat>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::Texture(const std::string& Xpos, const std::string& Xneg, const std::string& Ypos, const std::string& Yneg, const std::string& Zpos, const std::string& Zneg)
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

template<typename InternalFormat>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat>& Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::InitTexture(GLuint size, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "TextureCube: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = size;
		this->_height = size;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(size))) + 1;
		this->_levels = numLevels;
		this->_layers = 6;
		this->bind();
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, iFormat, size, size);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat>
void Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::InitSizeFromFile(const std::string& file, GLuint numLevels)
{
	SDL_Surface* loaded_img = IMG_Load(file.c_str());
	ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load file \"" + file + "\".").c_str());
	ASSERT(loaded_img->w == loaded_img->h, "TextureCube: wrong image size");

	InitTexture(loaded_img->w, numLevels);

	SDL_FreeSurface(loaded_img);
}

template<typename InternalFormat>
template<TextureType NewTexType, typename NewInternalFormat>
auto Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::MakeView(TexLevels levels)
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

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat> Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::operator[](TextureType face)
{
	ASSERT(detail::IsTextureTypeCubeSide(face), "TextureCube: face parameter has to be one of TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG}");
	return MakeFaceView(face);
}

template<typename InternalFormat>
Texture<TextureType::TEX_CUBE_MAP, InternalFormat> Texture<TextureType::TEX_CUBE_MAP, InternalFormat>::operator[](TexLevels levels)
{
	return MakeView(levels);
}