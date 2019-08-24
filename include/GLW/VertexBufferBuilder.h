#pragma once

#include "VertexBuffer.h"

#include <vector>

namespace GLWRAPPER_NS {

	template<typename VertexType>
	class VertexBufferBuilder {

		VertexBuffer& buffer;

	public:

		VertexBufferBuilder(VertexBuffer& buffer) : buffer(buffer) {}

		void LoadData(const std::vector<VertexType>& vertex_data) {
			VertexBufferData vbo_data;

			// create and activate VBO
			glGenBuffers(1, &vbo_data.vbo_id);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_data.vbo_id);

			// allocate memory for the active VBO and set its data
			vbo_data.count = GLsizei(vertex_data.size());
			glBufferData(
				GL_ARRAY_BUFFER,	
				sizeof(VertexType)*vertex_data.size(),
				vertex_data.data(),
				GL_STATIC_DRAW
			);

			buffer.Reset(std::move(vbo_data));
		}

	};

}