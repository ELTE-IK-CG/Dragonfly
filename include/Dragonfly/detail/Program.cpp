#include "Program.inl"
#include <vector>

using namespace df;


GLuint ProgramLowLevelBase::bound_program_id = 0;

bool ProgramLowLevelBase::link()
{
	ASSERT(program_id != 0, "Invalid Program");
	GL_CHECK;
	glLinkProgram(program_id);
	GL_CHECK;
	GLint result=0, loglen=0, errlen=0;
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &loglen);
	if (result == GL_FALSE || 0 != loglen  ){
		std::vector<char> error_message(loglen);
		glGetProgramInfoLog(program_id, loglen, &errlen, error_message.data());
		this->error_msg.append(error_message.data(), error_message.data() + errlen);
	}
	return result;
}

ProgramLowLevelBase::ProgramLowLevelBase()
	: program_id(glCreateProgram())
{
	ASSERT(program_id != 0, "Invalid Program");
}

ProgramLowLevelBase::~ProgramLowLevelBase(){
	if (program_id != 0)
		glDeleteProgram(program_id);
}

ProgramLowLevelBase::ProgramLowLevelBase(ProgramLowLevelBase && rhs)
{
	program_id = rhs.program_id;
	error_msg = std::move(rhs.error_msg);

	rhs.program_id = 0;
}

ProgramLowLevelBase & ProgramLowLevelBase::operator=(ProgramLowLevelBase && rhs)
{
	if (&rhs == this)
		return *this;

	program_id = rhs.program_id;
	error_msg = std::move(rhs.error_msg);

	rhs.program_id = 0;

	return *this;
}

const std::string& ProgramLowLevelBase::getErrors() const {
	return error_msg;
}
