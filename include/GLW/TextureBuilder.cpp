#include "TextureBuilder.h"

#include <SDL/SDL_image.h>

namespace GLWRAPPER_NS {

	namespace {

		int invert_image(int pitch, int height, void* image_pixels)
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

		int SDL_InvertSurface(SDL_Surface* image)
		{
			if (NULL == image)
			{
				SDL_SetError("Surface is NULL");
				return -1;
			}

			return invert_image(image->pitch, image->h, image->pixels);
		}
	}

	bool TextureBuilder::LoadFromFile(std::string path) {


		// Kép betöltése
		SDL_Surface* loaded_img = IMG_Load(path.c_str());

		if (loaded_img == nullptr)
		{
			return false;
		}

		// Convert to 32bit RGBA
		SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(loaded_img, SDL_PIXELFORMAT_RGBA32, 0);
		if (formattedSurf == nullptr)
		{
			SDL_FreeSurface(loaded_img);
			return false;
		}

		// Convert from SDL coordinates ( (0,0) topleft )  to OpenGL texture coordinates ( (0,0) bottomleft )
		if (SDL_InvertSurface(formattedSurf) == -1) {
			SDL_FreeSurface(formattedSurf);
			SDL_FreeSurface(loaded_img);
			return false;
		}

		_width = formattedSurf->w;
		_height = formattedSurf->h;

		auto size = formattedSurf->h*formattedSurf->pitch;
		data.resize(size);
		std::memcpy(data.data(), formattedSurf->pixels, size);

		// Free up SDL_Surface-s
		SDL_FreeSurface(formattedSurf);
		SDL_FreeSurface(loaded_img);


		return true;

	}

	void TextureBuilder::Generate(std::function<vec4(float u, float v)> fnc, int width, int height) {
		// TODO
	}

	void TextureBuilder::Build() {
		ResourceBuilder::Build();

		TextureData texture_data;
		//

		// Generate OpenGL texture
		glGenTextures(1, &texture_data.texture_id);

		glBindTexture(GL_TEXTURE_2D, texture_data.texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		// Generate mipmap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		//
		object.Reset(std::move(texture_data));
	}

}