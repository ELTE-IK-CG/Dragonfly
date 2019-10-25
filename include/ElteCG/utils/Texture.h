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
		switch (type)
		{
		case TextureType::TEX_CUBE_X_POS:
		case TextureType::TEX_CUBE_X_NEG:
		case TextureType::TEX_CUBE_Y_POS:
		case TextureType::TEX_CUBE_Y_NEG:
		case TextureType::TEX_CUBE_Z_POS:
		case TextureType::TEX_CUBE_Z_NEG:
			return true;
		default:
			return false;
		}
	}

	inline int invert_image(int pitch, int height, void* image_pixels)
	{
		int index;
		void* temp_row;
		int height_div_2;

		temp_row = (void *)malloc(pitch);
		if (NULL == temp_row)
		{
			SDL_SetError("Not enough memory for image inversion");
			return -1;
		}
		//if height is odd, don't need to swap middle row
		height_div_2 = (int)(height * .5);
		for (index = 0; index < height_div_2; index++) {
			//uses string.h
			memcpy((Uint8 *)temp_row,
				(Uint8 *)(image_pixels)+
				pitch * index,
				pitch);

			memcpy(
				(Uint8 *)(image_pixels)+
				pitch * index,
				(Uint8 *)(image_pixels)+
				pitch * (height - index - 1),
				pitch);
			memcpy(
				(Uint8 *)(image_pixels)+
				pitch * (height - index - 1),
				temp_row,
				pitch);
		}
		free(temp_row);
		return 0;
	}
}

class TextureLowLevelBase {
public:
	operator GLuint() { return texture_id; }
protected:
	GLuint texture_id = 0;

	TextureLowLevelBase() { glGenTextures(1, &texture_id); }
	~TextureLowLevelBase() { glDeleteTextures(1, &texture_id); }
};

template<typename InternalFormat, TextureType TexType>
class Texture{
	//Only specializations are allowed
	Texture() = delete;
};

template<typename InternalFormat, TextureType TexType>
class TextureBase : public TextureLowLevelBase
{
protected:
	int _width = 0, _height = 0, _depth = 0;
	TextureBase() = default;
	~TextureBase() = default;
	template<TextureType NewTexType, typename NewInternalFormat>
	Texture<NewInternalFormat, NewTexType> _MakeView(GLuint minLevel, GLuint numLevels, GLuint minLayer, GLuint numLayers)
	{
		Texture<NewInternalFormat, NewTexType> view;
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		static_assert(sizeof(InternalFormat) == sizeof(NewInternalFormat), "Texture: Internal formats must be of the same size class");
		glTextureView(view.texture_id, static_cast<GLenum>(NewTexType), this->texture_id, iFormat, minLevel, numLevels, minLayer, numLayers);
		GL_CHECK;
		return view;
	}
public:
	inline void bind() const { glBindTexture(static_cast<GLenum>(TexType), this->texture_id); }
	inline void bind(unsigned int hwSamplerUnit) const {
		ASSERT(hwSamplerUnit < 256, "Texture or sampler units you can attach your texture to start from 0 (and go to 96 minimum in OpenGL 4.5.)");
		glActiveTexture(GL_TEXTURE0 + hwSamplerUnit); glBindTexture(static_cast<GLenum>(TexType), this->texture_id); }
};


template<typename InternalFormat>
class Texture<InternalFormat, TextureType::TEX_2D> : public TextureBase<InternalFormat, TextureType::TEX_2D>
{
	using Base = TextureBase<InternalFormat, TextureType::TEX_2D>;

	void InitTexture(int width, int height, int levels = 1)
	{
		this->_width = width;
		this->_height = height;
		this->_depth = levels;
		ASSERT(width >= 1 && height >= 1 && levels >= 1 && levels <= log2(width > height ? width : height) + 1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_2D, levels, iFormat, width, height);
	}

public:
	Texture(int width, int height, int levels = 1)
	{
		InitTexture(width, height, levels);
	}

	template<TextureType NewTexType = TextureType::TEX_2D, typename NewInternalFormat = InternalFormat>
	Texture<NewInternalFormat, NewTexType> MakeView(GLuint minLevel = 0, GLuint numLevels = -1)
	{
		static_assert(NewTexType == TextureType::TEX_2D || NewTexType == TextureType::TEX_2D_ARRAY, "Texture2D: Incompatible view target.");
		if (numLevels == -1) numLevels = this->_depth - minLevel;
		ASSERT(minLevel < this->_depth, "Texture2D: Too large mipmap index.");
		WARNING(minLevel + numLevels > this->_depth, "Texture2D: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use -1.");
		return this->_MakeView<NewInternalFormat, NewTexType>(minLevel, numLevels, 0, 1);
	}

	Texture(const std::string &file, bool invertImage = true, int levels = -1)
	{
		SDL_Surface* loaded_img = IMG_Load(file.c_str());
		ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());
		
		if(levels == -1) levels = static_cast<int>(floor(log2(loaded_img->w > loaded_img->h ? loaded_img->w : loaded_img->h))) + 1;
		InitTexture(loaded_img->w, loaded_img->h, levels);

		GLenum sdl_pxformat = GL_UNSIGNED_BYTE; //todo calculate from format
		Uint32 sdl_format = loaded_img->format->BytesPerPixel == 3 ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;
		GLenum sdl_channels = loaded_img->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
		if (loaded_img->format->format != sdl_format) {
			SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(loaded_img, sdl_format, 0);
			SDL_FreeSurface(loaded_img);
			ASSERT(formattedSurf != nullptr, "Texture2D: Failed to convert texture type.");
			loaded_img = formattedSurf;
		}

		if (invertImage) {
			int ret = detail::invert_image(loaded_img->pitch, loaded_img->h, loaded_img->pixels);
			ASSERT(ret == 0, "Texture2D: Failed to invert image");
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->_width, this->_height, sdl_channels, sdl_pxformat, static_cast<void*>(loaded_img->pixels));

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(loaded_img);
	}
};

template<typename InternalFormat>
class Texture<InternalFormat, TextureType::TEX_CUBE_MAP> : public TextureBase<InternalFormat, TextureType::TEX_CUBE_MAP>
{
	using Base = TextureBase<InternalFormat, TextureType::TEX_CUBE_MAP>;
	void InitTexture(int size, int levels = 1)
	{
		if (levels == -1) levels = static_cast<int>(floor(log2(size))) + 1;
		this->bind();
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, levels, iFormat, size, size);
	}
public:
	Texture(GLint size, GLint levels = -1)
	{
		InitTexture(size, levels);
	}
	Texture(const std::string& Xpos, const std::string& Xneg, const std::string& Ypos, const std::string& Yneg, const std::string& Zpos, const std::string& Zneg)
	{
		InitTexture();
	}

	
	template<TextureType NewTexType = TextureType::TEX_CUBE_MAP, typename NewInternalFormat = InternalFormat>
	auto MakeView(GLuint minLevel = 0, GLuint numLevels = -1)
	{
		static_assert(NewTexType != TextureType::TEX_2D, "TextureCube: Use the TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG} instead of TextureType::TEX_2D.");
		static_assert(/*NewTexType == TextureType::TEX_2D || */    NewTexType == TextureType::TEX_2D_ARRAY
			|| NewTexType == TextureType::TEX_CUBE_MAP			|| NewTexType == TextureType::TEX_CUBE_MAP_ARRAY
			|| detail::IsTextureTypeCubeSide(NewTexType)		, "TextureCube: Incompatible view target.");
		GLuint minLayers = 0, numLayers = 6;
		if constexpr (detail::IsTextureTypeCubeSide(NewTexType))
		{
			minLayers = static_cast<GLuint>(NewTexType) - static_cast<GLuint>(TextureType::TEX_CUBE_X_POS);
			numLayers = 1;
		}
		if (numLevels == -1) numLevels = this->_depth - minLevel;
		ASSERT(minLevel < this->_depth, "TextureCube: Too large mipmap index.");
		WARNING(minLevel + numLevels > this->_depth, "TextureCube: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use -1.");
		if constexpr (detail::IsTextureTypeCubeSide(NewTexType))
			return this->_MakeView<NewInternalFormat, TextureType::TEX_2D>, NewTexType>(minLevel, numLevels, minLayers, numLayers);
		else
			return this->_MakeView<NewInternalFormat, NewTexType>(minLevel, numLevels, minLayers, numLayers);

	}

};

template<typename InternalFormat = glm::u8vec3>
using Texture2D = Texture<InternalFormat, TextureType::TEX_2D>;
template<typename InternalFormat = glm::u8vec3>
using TextureCube = Texture<InternalFormat, TextureType::TEX_CUBE_MAP>;