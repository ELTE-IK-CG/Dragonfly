#pragma once
#include <string>
#include <SDL/SDL_image.h>
#include "../../config.h"
#include "Texture.h"

namespace df
{

template<typename InternalFormat>
class Texture<TextureType::TEX_3D, InternalFormat> : public TextureBase<TextureType::TEX_3D, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_3D, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

public:
	Texture() {}
	Texture(GLuint width, GLuint height, GLuint depth, GLuint numLevels = ALL);
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o);

	template<typename Format>
	Texture& operator= (const std::vector<Format>& data);

	void InitTexture(GLuint width, GLuint height, GLuint depth, GLuint numLevels = ALL);

	template<typename Format>
	Texture& LoadData(const std::vector<Format>& data, bool genMipmap = true);

	template<TextureType NewTexType = TextureType::TEX_3D, typename NewInternalFormat = InternalFormat>
	Texture<NewTexType, NewInternalFormat> MakeView(TexLevels levels = 0_levelAll);

	Texture operator[] (TexLevels levels);
};

template<typename InternalFormat>
Texture<TextureType::TEX_3D, InternalFormat>::Texture(GLuint width, GLuint height, GLuint depth, GLuint numLevels)
{
	InitTexture(width, height, depth, numLevels);
}

template<typename InternalFormat>
Texture<TextureType::TEX_3D, InternalFormat>& Texture<TextureType::TEX_3D, InternalFormat>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_3D, InternalFormat>& Texture<TextureType::TEX_3D, InternalFormat>::operator= (const std::vector<Format>& data)
{
	return LoadData(data);
}

template<typename InternalFormat>
void Texture<TextureType::TEX_3D, InternalFormat>::InitTexture(GLuint width, GLuint height, GLuint depth, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "Texture3D: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = width;
		this->_height = height;
		this->_depth = depth;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(width > height ? (depth > width ? depth : width) : height))) + 1;
		this->_levels = numLevels;
		this->_layers = 1;
		ASSERT(width >= 1 && height >= 1 && depth >= 1 && numLevels >= 1 && numLevels <= log2(width > height ? (depth > width ? depth : width) : height) + 1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = detail::getInternalFormat<InternalFormat>();
		glTexStorage3D(GL_TEXTURE_3D, numLevels, iFormat, width, height, depth);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_3D, InternalFormat>& Texture<TextureType::TEX_3D, InternalFormat>::LoadData(const std::vector<Format>& data, bool genMipmap)
{
	ASSERT(this->_hasStorage, "Texture3D: you can only load data to a texture if it has its size set (Init)");
	if (this->_hasStorage) {
		this->bind();

		constexpr GLenum pxFormat = detail::getInternalChannel<Format>();
		constexpr GLenum pxType   = detail::getInternalBaseType<Format>();

		ASSERT(this->_width * this->_height * this->_depth == data.size(), "Texture3D: trying to write a texture with the wrong amount of data");
		if (this->_width * this->_height * this->_depth > data.size())
			return *this;

		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, this->_width, this->_height, this->_depth, pxFormat, pxType, static_cast<const void*>(&data[0]));

		if (genMipmap)
			glGenerateMipmap(GL_TEXTURE_3D);
	}
	return *this;
}

template<typename InternalFormat>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> Texture<TextureType::TEX_3D, InternalFormat>::MakeView(TexLevels levels)
{
	static_assert(NewTexType == TextureType::TEX_3D, "Texture3D: Incompatible view target.");
	if (levels.num == ALL) levels.num = this->_levels - levels.min;
	ASSERT(levels.min < this->_levels, "Texture3D: Too large mipmap index.");
	WARNING(levels.min + levels.num > this->_levels, "Texture3D: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	return this->_MakeView<NewTexType, NewInternalFormat>(levels, 0_layer);
}

template<typename InternalFormat>
Texture<TextureType::TEX_3D, InternalFormat> Texture<TextureType::TEX_3D, InternalFormat>::operator[] (TexLevels levels)
{
	return MakeView(levels);
}

} //namespace df
