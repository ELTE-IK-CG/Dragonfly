#pragma once

#include "IndexBuffer.h"

#include <vector>

namespace GLWRAPPER_NS {

	template<typename IndexType=GLuint>
	class IndexBufferBuilder {

		IndexBuffer& buffer;

	public:

		IndexBufferBuilder(IndexBuffer& buffer) : buffer(buffer) {}

		void LoadData(const std::vector<IndexType>& indices) {
			IndexBufferData ibo_data;

			// create and activate VBO
			glGenBuffers(1, &ibo_data.ibo_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data.ibo_id);

			// allocate memory for the active VBO and set its data
			ibo_data.count = GLsizei(indices.size());
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,	
				sizeof(IndexType)*indices.size(),
				indices.data(),
				GL_STATIC_DRAW
			);

			buffer.Reset(std::move(ibo_data));
		}

	};

}