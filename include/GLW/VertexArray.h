#pragma once

#include "ResourceOwner.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <functional>

namespace GLWRAPPER_NS {

	struct VertexArrayData {
		GLuint vao_id = 0;

		VertexBuffer vbo;
		IndexBuffer ibo;

		VertexArrayData() {}

		void Reset() {
			if (vao_id > 0) {
				glDeleteVertexArrays(1, &vao_id);
			}

			vbo.Reset();
			ibo.Reset();
		}
	};

	class VertexArray : public ResourceOwner<VertexArrayData> {

	public:
		VertexArray() : ResourceOwner() {}

		void Bind() {
			glBindVertexArray(Get().vao_id);
		}

		void Unbind() {
			glBindVertexArray(0);
		}

		void Bind(std::function<void(VertexArray&)> fnc) {
			Bind();

			fnc(*this);

			Unbind();
		}

		void DrawArrays(GLenum mode, GLint first, GLsizei count) {
			assert(GetCurrentVaoId() == Get().vao_id);

			glDrawArrays(mode, first, count);
		}

		void DrawArrays(GLenum mode = GL_TRIANGLES, GLint first = 0) {
			assert(GetCurrentVaoId() == Get().vao_id);

			DrawArrays(mode, first, Get().vbo.GetCount());
		}

		void DrawIndexed(GLenum mode, GLsizei count) {
			glDrawElements(mode, count, GL_UNSIGNED_INT, NULL);
		}

		void DrawIndexed(GLenum mode = GL_TRIANGLES) {
			DrawIndexed(mode, Get().ibo.GetCount());
		}

	private:

		// Debug
		GLuint GetCurrentVaoId() const {
			GLint current_vao;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
			return GLuint(current_vao);
		}

	};

}