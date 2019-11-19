#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL/SDL_image.h>
#include "../../config.h"
#include "Texture.h"
#include "../Traits/texture_helper.h"

namespace df
{

template<typename InternalFormat>
class Texture<TextureType::TEX_2D_ARRAY, InternalFormat> : public TextureBase<TextureType::TEX_2D_ARRAY, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_2D_ARRAY, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

public:
	Texture() {}
	Texture(GLuint width, GLuint height, GLuint numLayers, GLuint numLevels = ALL);
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o);

	template<typename Format>
	Texture& operator= (const std::vector<Format>& data);

	void InitTexture(GLuint width, GLuint height, GLuint numLayers, GLuint numLevels = ALL);

	template<typename Format>
	Texture& LoadData(const std::vector<Format>& data, bool genMipmap = true);

	template<TextureType NewTexType = TextureType::TEX_2D_ARRAY, typename NewInternalFormat = InternalFormat>
	Texture<NewTexType, NewInternalFormat> MakeView(TexLevelsAndLayers levelsAndLayers = 0_levelAll & 0_layerAll);

	Texture operator[] (TexLevelsAndLayers levelsAndLayers);
	Texture<TextureType::TEX_2D, InternalFormat> operator[] (GLuint layer);
};

template<typename InternalFormat>
Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::Texture(GLuint width, GLuint height, GLuint numLayers, GLuint numLevels)
{
	InitTexture(width, height, numLayers, numLevels);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D_ARRAY, InternalFormat>& Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::operator= (Texture&& _o) {
	Base::operator=(std::move(_o));
	return *this;
}

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_2D_ARRAY, InternalFormat>& Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::operator= (const std::vector<Format>& data)
{
	return LoadData(data);
}

template<typename InternalFormat>
void Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::InitTexture(GLuint width, GLuint height, GLuint numLayers, GLuint numLevels)
{
	ASSERT(!this->_hasStorage, "Texture2DArray: cannot change texture's size after the storage has been set");
	if (!this->_hasStorage) {
		this->_width = width;
		this->_height = height;
		this->_depth = 1;
		if (numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(width > height ? width : height))) + 1;
		this->_levels = numLevels;
		this->_layers = numLayers;
		ASSERT(width >= 1 && height >= 1 && numLevels >= 1 && numLevels <= log2(width > height ? width : height) + 1 && numLayers >= 1, "Texture2DArray: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, numLevels, iFormat, width, height, numLayers);
		this->_hasStorage = true;
	}
}

template<typename InternalFormat>
template<typename Format>
Texture<TextureType::TEX_2D_ARRAY, InternalFormat>& Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::LoadData(const std::vector<Format>& data, bool genMipmap)
{
	ASSERT(this->_hasStorage, "Texture2DArray: you can only load data to a texture if it has its size set (Init)");
	if (this->_hasStorage) {
		this->bind();

		constexpr GLenum pxFormat = eltecg::ogl::helper::getInternalChannel<Format>();
		constexpr GLenum pxType = eltecg::ogl::helper::getInternalBaseType<Format>();

		ASSERT(this->_width * this->_height * this->_numLayers == data.size(), "Texture2DArray: trying to write a texture with the wrong amount of data");
		if (this->_width * this->_height * this->_numLayers > data.size())
			return *this;

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, this->_width, this->_height, this->_numLayers, pxFormat, pxType, static_cast<const void*>(&data[0]));

		if (genMipmap)
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}
	return *this;
}

template<typename InternalFormat>
template<TextureType NewTexType, typename NewInternalFormat>
Texture<NewTexType, NewInternalFormat> Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::MakeView(TexLevelsAndLayers levelsAndLayers)
{
	static_assert(NewTexType == TextureType::TEX_2D || NewTexType == TextureType::TEX_2D_ARRAY, "Texture2DArray: Incompatible view target.");
	if (levelsAndLayers.levels.num == ALL) levelsAndLayers.levels.num = this->_levels - levelsAndLayers.levels.min;
	if (levelsAndLayers.layers.num == ALL) levelsAndLayers.layers.num = this->_layers - levelsAndLayers.layers.min;
	ASSERT(levelsAndLayers.levels.min < this->_levels, "Texture2DArray: Too large mipmap index.");
	ASSERT(levelsAndLayers.layers.min < this->_layers, "Texture2DArray: Too large layer index.");
	WARNING(levelsAndLayers.levels.min + levelsAndLayers.levels.num > this->_levels, "Texture2DArray: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
	WARNING(levelsAndLayers.layers.min + levelsAndLayers.layers.num > this->_layers, "Texture2DArray: Number of layers must be more than intended.");
	return this->_MakeView<NewTexType, NewInternalFormat>(levelsAndLayers.levels, levelsAndLayers.layers);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D_ARRAY, InternalFormat> Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::operator[] (TexLevelsAndLayers levelsAndLayers)
{
	return MakeView(levelsAndLayers);
}

template<typename InternalFormat>
Texture<TextureType::TEX_2D, InternalFormat> Texture<TextureType::TEX_2D_ARRAY, InternalFormat>::operator[] (GLuint layer)
{
	return MakeView<TextureType::TEX_2D>(TexLayers{ layer, 1 });
}

} //namespace df
