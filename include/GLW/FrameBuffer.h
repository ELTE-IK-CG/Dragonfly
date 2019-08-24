#pragma once

#include "ResourceOwner.h"
#include "Texture.h"

#include <vector>
#include <functional>

namespace GLWRAPPER_NS {

	struct FrameBufferData {
		GLuint fbo_id;

		Texture depth_buffer;
		std::vector<Texture> color_buffers;

		FrameBufferData() : fbo_id(0) {}

		void Reset() {
			if (fbo_id > 0) {
				glDeleteFramebuffers(1, &fbo_id);
			}
		}
	};

	class FrameBuffer : public ResourceOwner<FrameBufferData> {
	public:
		FrameBuffer(): ResourceOwner() {}

		const Texture& GetColorBuffer(int index) {
			return Get().color_buffers[index];
		}

		void Bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, Get().fbo_id);
		}

		void Unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Bind(std::function<void(FrameBuffer&)> fnc) {
			Bind();

			fnc(*this);

			Unbind();
		}
	};

}