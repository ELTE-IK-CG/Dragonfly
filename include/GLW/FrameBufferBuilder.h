#pragma once

#include "FrameBuffer.h"
#include "ResourceBuilder.h"

namespace GLWRAPPER_NS {

	struct FrameBufferAttachment {
		GLint internalformat;
		GLenum format;
		GLenum type;

		static FrameBufferAttachment Float4() {
			return {
				GL_RGBA32F,
				GL_RGBA,
				GL_FLOAT
			};
		}

		static FrameBufferAttachment Byte4() {
			return {
				GL_RGBA,
				GL_RGBA,
				GL_UNSIGNED_BYTE
			};
		}
	};

	class FrameBufferBuilder : public ResourceBuilder<FrameBuffer> {

		GLsizei _width = 0;
		GLsizei _height = 0;

		std::vector<FrameBufferAttachment> attachments;

	public:
		FrameBufferBuilder(FrameBuffer& fbo) : ResourceBuilder(fbo) {}

		void SetSize(GLsizei width, GLsizei height) {
			_width = width;
			_height = height;
		}

		void AddAttachment(FrameBufferAttachment att) {
			attachments.push_back(att);
		}

		void Build() override {
			ResourceBuilder::Build();

			assert(_width > 0);
			assert(_height > 0);

			FrameBufferData fbo_data;

			glGenFramebuffers(1, &fbo_data.fbo_id);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_data.fbo_id);

			for (int i = 0; i < attachments.size(); i++) {
				auto att = attachments[i];

				// TODO: TextureBuilder?
				Texture color_attachment;
				TextureData att_data;

				glGenTextures(1, &att_data.texture_id);
				glBindTexture(GL_TEXTURE_2D, att_data.texture_id);

				glTexImage2D(GL_TEXTURE_2D, 0, att.internalformat, _width, _height, 0, att.format, att.type, nullptr);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, att_data.texture_id, 0);

				color_attachment.Reset(std::move(att_data));
				fbo_data.color_buffers.push_back(std::move(color_attachment));
			}

			{
				TextureData depth_data;

				glGenTextures(1, &depth_data.texture_id);
				glBindTexture(GL_TEXTURE_2D, depth_data.texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_data.texture_id, 0);

				fbo_data.depth_buffer.Reset(std::move(depth_data));
			}

			// Unbind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Finish
			object.Reset(std::move(fbo_data));

		}
	};

}