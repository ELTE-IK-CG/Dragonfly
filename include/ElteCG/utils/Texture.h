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

// TODO namespace
static const GLuint ALL = -1; // Mipmap level / array layer

// Texture Mipmap levels
struct TexLevels {
	GLuint min;
	GLuint num;

	constexpr TexLevels operator>> (GLuint numLelvels) {
		return TexLevels{ this->min, numLelvels };
	}
};
constexpr TexLevels operator"" _level(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), 1 }; }
constexpr TexLevels operator"" _levelAll(unsigned long long int minLevel) { return TexLevels{ static_cast<GLuint>(minLevel), ALL }; }

// Texture Array layers
struct TexLayers {
	GLuint min;
	GLuint num;

	constexpr TexLayers operator>> (GLuint numLayers) {
		return TexLayers{ this->min, numLayers };
	}
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
	TextureBase& operator= (TextureBase&& _o) {
		TextureLowLevelBase::operator=(std::move(_o));
		return *this;
	}

	template<TextureType NewTexType, typename NewInternalFormat>
	Texture<NewTexType, NewInternalFormat> _MakeView(TexLevels levels, TexLayers layers)
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
public:
	inline void bind() const { glBindTexture(static_cast<GLenum>(TexType), this->texture_id); }
	inline void bind(GLuint hwSamplerUnit) const {
		ASSERT(hwSamplerUnit < 256, "Texture or sampler units you can attach your texture to start from 0 (and go to 96 minimum in OpenGL 4.5.)");
		glActiveTexture(GL_TEXTURE0 + hwSamplerUnit);
		this->bind();
	}
};


template<typename InternalFormat>
class Texture<TextureType::TEX_2D, InternalFormat> : public TextureBase<TextureType::TEX_2D, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_2D, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

public:
	bool invertYOnFileLoad = true;
private:

	void LoadFromSDLSurface(SDL_Surface* img)
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(img);
	}

public:
	Texture() {}
	Texture(GLuint width, GLuint height, GLuint numLevels = ALL, bool invertImage = true)
		: invertYOnFileLoad(invertImage)
	{
		InitTexture(width, height, numLevels);
	}
	Texture(const std::string &file, GLuint numLevels = ALL, bool invertImage = true)
		: invertYOnFileLoad(invertImage)
	{
		SDL_Surface* loaded_img = IMG_Load(file.c_str());
		ASSERT(loaded_img != nullptr, ("Texture2D: Failed to load texture from \"" + file + "\".").c_str());
		
		InitTexture(loaded_img->w, loaded_img->h, numLevels);

		LoadFromSDLSurface(loaded_img);
	}
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o) {
		Base::operator=(std::move(_o));
		return *this;
	}

	Texture<TextureType::TEX_2D, InternalFormat>& operator= (const std::string& file)
	{
		return LoadFromFile(file);
	}

	void InitTexture(GLuint width, GLuint height, GLuint numLevels = ALL)
	{
		ASSERT(!this->_hasStorage, "Texture2D: cannot change texture's size after the storage has been set");
		if (!this->_hasStorage) {
			this->_width = width;
			this->_height = height;
			if(numLevels == ALL) numLevels = static_cast<GLuint>(floor(log2(width > height ? width : height))) + 1;
			this->_levels = numLevels;
			this->_layers = 1;
			ASSERT(width >= 1 && height >= 1 && numLevels >= 1 && numLevels <= log2(width > height ? width : height) + 1, "Texture2D: Invalid dimensions");
			this->bind(); // todo named
			constexpr GLenum iFormat = eltecg::ogl::helper::getInternalFormat<InternalFormat>();
			glTexStorage2D(GL_TEXTURE_2D, numLevels, iFormat, width, height);
			this->_hasStorage = true;
		}
	}

	Texture<TextureType::TEX_2D, InternalFormat>& LoadFromFile(const std::string& file, bool invertImage = true)
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

	template<TextureType NewTexType = TextureType::TEX_2D, typename NewInternalFormat = InternalFormat>
	Texture<NewTexType, NewInternalFormat> MakeView(TexLevels levels = 0_levelAll)
	{
		static_assert(NewTexType == TextureType::TEX_2D || NewTexType == TextureType::TEX_2D_ARRAY, "Texture2D: Incompatible view target.");
		if (levels.num == ALL) levels.num = this->_levels - levels.min;
		ASSERT(levels.min < this->_levels, "Texture2D: Too large mipmap index.");
		WARNING(levels.min + levels.num > this->_levels, "Texture2D: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
		return this->_MakeView<NewTexType, NewInternalFormat>(levels, 0_layer);
	}

	Texture<TextureType::TEX_2D, InternalFormat> operator[] (TexLevels levels)
	{
		return MakeView(levels);
	}
};

template<typename InternalFormat>
class Texture<TextureType::TEX_CUBE_MAP, InternalFormat> : public TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat>
{
	using Base = TextureBase<TextureType::TEX_CUBE_MAP, InternalFormat>;

	template<TextureType TT, typename IF>
	friend class TextureBase;

	void LoadFromSDLSurface(SDL_Surface* img, TextureType side)
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

	template<typename NewInternalFormat = InternalFormat>
	Texture<TextureType::TEX_2D, NewInternalFormat> MakeFaceView(TextureType face, TexLevels levels = 0_levelAll)
	{
		TexLayers layers{ static_cast<GLuint>(face) - static_cast<GLuint>(TextureType::TEX_CUBE_X_POS), 1 };
		if (levels.num == ALL) levels.num = this->_levels - levels.min;
		ASSERT(levels.min < this->_levels, "TextureCube: Too large mipmap index.");
		WARNING(levels.min + levels.num > this->_levels, "TextureCube: Number of mipmap levels must be more than intended. For maximum available mipmap levels, use ALL.");
		auto tex = this->_MakeView<TextureType::TEX_2D, NewInternalFormat>(levels, layers);
		tex.invertYOnFileLoad = false;
		return tex;
	}

public:
	Texture() {}
	Texture(GLint size, GLint numLevels = ALL)
	{
		InitTexture(size, numLevels);
	}
	Texture(const std::string& Xpos, const std::string& Xneg, const std::string& Ypos, const std::string& Yneg, const std::string& Zpos, const std::string& Zneg)
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
	~Texture() {}

	Texture(const Texture&) = delete;
	Texture(Texture&& _o) : Base(std::move(_o)) {}

	Texture& operator= (const Texture&) = delete;
	Texture& operator= (Texture&& _o) {
		Base::operator=(std::move(_o));
		return *this;
	}

	void InitTexture(GLuint size, GLuint numLevels = ALL)
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
	void InitSizeFromFile(const std::string& file, GLuint numLevels = ALL)
	{
		SDL_Surface* loaded_img = IMG_Load(file.c_str());
		ASSERT(loaded_img != nullptr, ("TextureCube: Failed to load file \"" + file + "\".").c_str());
		ASSERT(loaded_img->w == loaded_img->h, "TextureCube: wrong image size");

		InitTexture(loaded_img->w, numLevels);

		SDL_FreeSurface(loaded_img);
	}
	
	template<TextureType NewTexType = TextureType::TEX_CUBE_MAP, typename NewInternalFormat = InternalFormat>
	auto MakeView(TexLevels levels = 0_levelAll)
	{
		static_assert(NewTexType != TextureType::TEX_2D, "TextureCube: Use the TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG} instead of TextureType::TEX_2D.");
		static_assert(/*NewTexType == TextureType::TEX_2D || */    NewTexType == TextureType::TEX_2D_ARRAY
			|| NewTexType == TextureType::TEX_CUBE_MAP			|| NewTexType == TextureType::TEX_CUBE_MAP_ARRAY
			|| detail::IsTextureTypeCubeSide(NewTexType)		, "TextureCube: Incompatible view target.");
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

	Texture<TextureType::TEX_2D, InternalFormat> operator[](TextureType face)
	{
		ASSERT(detail::IsTextureTypeCubeSide(face), "TextureCube: face parameter has to be one of TextureType::TEX_CUBE_{X,Y,Z}_{POS,NEG}");
		return MakeFaceView(face);
	}

	Texture<TextureType::TEX_CUBE_MAP, InternalFormat> operator[](TexLevels levels)
	{
		return MakeView(levels);
	}
};

template<typename InternalFormat = glm::u8vec3>
using Texture2D = Texture<TextureType::TEX_2D, InternalFormat>;
template<typename InternalFormat = glm::u8vec3>
using TextureCube = Texture<TextureType::TEX_CUBE_MAP, InternalFormat>;