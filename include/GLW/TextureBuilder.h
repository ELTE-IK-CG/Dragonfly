#pragma once

#include "Texture.h"

#include "ResourceBuilder.h"

#include <cstdint>
#include <vector>
#include <string>
#include <functional>

namespace GLWRAPPER_NS {

	class TextureBuilder: public ResourceBuilder<Texture> {

		std::vector<uint8_t> data;

		GLsizei _width = 0;
		GLsizei _height = 0;

	public:

		TextureBuilder(Texture& texture) : ResourceBuilder(texture) {}

		bool LoadFromFile(std::string path);
		void Generate(std::function<vec4(float u, float v)> fnc, int width, int height);

		void Build() override;

	};

}