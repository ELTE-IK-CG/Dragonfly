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
};

class TextureLowLevelBase {
protected:
	GLuint texture_id = 0;

	TextureLowLevelBase() { glGenTextures(1, &texture_id); }
	~TextureLowLevelBase() { glDeleteTextures(1, &texture_id); }
};

template<typename InternalFormat, TextureType TexType>
class TextureBase : protected TextureLowLevelBase
{
protected:
	int _width = 0, _height = 0, _depth = 0;
	TextureBase(int width, int height, int depth) : _width(width), _height(height), _depth(depth) {}
	TextureBase() = default;
	~TextureBase() = default;
public:
	inline void bind() const { glBindTexture(static_cast<GLenum>(TexType), this->texture_id); }
	inline void bind(unsigned int hwSamplerUnit) const {
		ASSERT(hwSamplerUnit < 256, "Texture or sampler units you can attach your texture to start from 0 (and go to 96 minimum in OpenGL 4.5.)");
		glActiveTexture(GL_TEXTURE0 + hwSamplerUnit); glBindTexture(static_cast<GLenum>(TexType), this->texture_id); }
};

template<typename InternalFormat, TextureType TexType>
class Texture{
	//Only specializations are allowed
};

template<typename InternalFormat>
class Texture<InternalFormat, TextureType::TEX_2D> : public TextureBase<InternalFormat, TextureType::TEX_2D>
{
	using Base = TextureBase<InternalFormat, TextureType::TEX_2D>;
public:
	Texture(int width, int height, int levels = 1) : Base(width, height, levels)
	{
		ASSERT(width >= 1 && height >= 1 && levels >= 1 && levels <= log2(width > height ? width : height)+1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_2D, levels, iFormat, width, height);
	}

	Texture(const std::string &file, int levels = -1)
	{
		SDL_Surface* loaded_img = IMG_Load(file.c_str());
		ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());
		
		if(levels == -1) levels = static_cast<int>(floor(log2(loaded_img->w > loaded_img->h ? loaded_img->w : loaded_img->h))) + 1;
		this->_width = loaded_img->w;
		this->_height = loaded_img->h;
		this->_depth = levels;
		ASSERT(this->_width >= 1 && this->_height >= 1 && levels >= 1 && levels <= log2(this->_width > this->_height ? this->_width : this->_height)+1, "Texture2D: Invalid dimensions");
		this->bind(); // todo named
		constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
		glTexStorage2D(GL_TEXTURE_2D, levels, iFormat, this->_width, this->_height);

		GLenum sdl_channels = SDL_BYTEORDER == SDL_LIL_ENDIAN ? (loaded_img->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR) : (loaded_img->format->BytesPerPixel == 4 ? GL_RGB : GL_RGB);
		GLenum sdl_pxformat = GL_UNSIGNED_BYTE; //todo calculate from format

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->_width, this->_height, sdl_channels, sdl_pxformat, static_cast<void*>(loaded_img->pixels));

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(loaded_img);
	}
};

template<typename InternalFormat = glm::u8vec3>
using Texture2D = Texture<InternalFormat, TextureType::TEX_2D>;