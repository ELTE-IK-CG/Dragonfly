#pragma once

#include "SharedResourceOwner.h"

namespace GLWRAPPER_NS {

	struct IndexBufferData {
		GLuint ibo_id = 0;
		GLsizei count = 0;

		IndexBufferData() {}
		IndexBufferData(GLuint ibo_id) : ibo_id(ibo_id) {}

		void Reset() {
			if (ibo_id > 0) {
				glDeleteBuffers(1, &ibo_id);
			}
		}
	};

	// One IBO can be used by multiple VAO
	class IndexBuffer : public SharedResourceOwner<IndexBufferData> {

	public:

		IndexBuffer() : SharedResourceOwner() {}

		GLuint GetId() const {
			return Get().ibo_id;
		}

		GLsizei GetCount() const {
			return Get().count;
		}

		template<typename V>
		static IndexBuffer FromVector(V& vector);

	};

}

#include "IndexBufferImpl.h"