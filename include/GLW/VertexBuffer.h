#pragma once

#include "SharedResourceOwner.h"

namespace GLWRAPPER_NS {

	struct VertexBufferData {
		GLuint vbo_id = 0;
		GLsizei count = 0;

		VertexBufferData() {}
		VertexBufferData(GLuint vbo_id) : vbo_id(vbo_id) {}

		void Reset() {
			if (vbo_id > 0) {
				glDeleteBuffers(1, &vbo_id);
			}
		}
	};

	// One VBO can be used by multiple VAO
	class VertexBuffer : public SharedResourceOwner<VertexBufferData> {

	public:

		VertexBuffer() : SharedResourceOwner() {}

		GLuint GetId() const {
			return Get().vbo_id;
		}

		GLsizei GetCount() const {
			return Get().count;
		}

		template<typename V>
		static VertexBuffer FromVector(V& vector);

	};

}

#include "VertexBufferImpl.h"