#include "Texture.h"
#include "TextureBuilder.h"

namespace GLWRAPPER_NS {

	Texture Texture::FromFile(std::string path) {
		Texture img;

		TextureBuilder builder(img);
		builder.LoadFromFile(path);
		builder.Build();

		return img;
	}

}