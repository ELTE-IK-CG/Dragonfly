#pragma once
#include "ProgramFwd.h"
#include "../Framebuffer/FramebufferBase.h"
#include "../Vao/Vao.h"
#include <GL/glew.h>
#include <string>

namespace df
{

	class ProgramLowLevelBase
	{
	protected:
		static GLuint bound_program_id;
		GLuint program_id = 0;
		std::string error_msg;
		bool link();
		inline void bind() {
			if (bound_program_id != program_id) {
				glUseProgram(program_id);
				bound_program_id = program_id;
			}
		}
		ProgramLowLevelBase();
		~ProgramLowLevelBase();

		ProgramLowLevelBase(const ProgramLowLevelBase&) = delete;
		ProgramLowLevelBase& operator=(const ProgramLowLevelBase&) = delete;

		ProgramLowLevelBase(ProgramLowLevelBase&& rhs);
		ProgramLowLevelBase& operator=(ProgramLowLevelBase&& rhs);

		template<typename Shader_t>
		void attachShader(const Shader_t& sh) {
			if (sh.getID() != 0) glAttachShader(program_id, sh.getID());
		}
		FramebufferBase framebuffer;

		inline void draw(const VaoElements& vao);
		inline void draw(const VaoArrays& vao);

	public:
		struct LinkType {};	//contains nothing at all
		const std::string& getErrors() const;
	};

	inline void ProgramLowLevelBase::draw(const VaoArrays& vao)
	{
		framebuffer.bind();	this->bind(); vao.bind();
		glDrawArrays(vao._mode, vao._first, vao._count);
	}
	inline void ProgramLowLevelBase::draw(const VaoElements& vao)
	{
		framebuffer.bind();	this->bind(); vao.bind();
		glDrawElements(vao._mode, vao._count, vao._ibo, nullptr);
	}

} //namespace df

