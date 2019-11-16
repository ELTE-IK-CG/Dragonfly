#if defined(_DEBUG) || defined(GPU_DEBUG)

//used to be debug.cpp

#include "..\config.h"
#include <stdio.h>
#include <GL/glew.h>

#ifdef USE_COLOR_CODES
#include <windows.h>

constexpr WORD W_F_I = 0b00001000     ;
constexpr WORD W_F_R = 0b00000100     ;
constexpr WORD W_F_G = 0b00000010     ;
constexpr WORD W_F_B = 0b00000001     ;
constexpr WORD W_B_I = 0b10000000     ;
constexpr WORD W_B_R = 0b01000000     ;
constexpr WORD W_B_G = 0b00100000     ;
constexpr WORD W_B_B = 0b00010000     ;


constexpr WORD warning_col		= W_F_R | W_F_G;	//yellow
constexpr WORD assert_col		= W_F_R;			//red
constexpr WORD gpu_warning_col	= W_F_B | W_F_G;	//cyan
constexpr WORD gpu_assert_col	= W_F_R | W_F_B;	//magenta

const WORD colors[] = { warning_col,assert_col,gpu_warning_col,gpu_assert_col };

#endif //USE_COLOR_CODES

const char* message_typenames[] = {"WARNING", "ASSERTION FAILED", "GPU WARNING", "GPU ASSERTION FAILED"};
const char* file_pos_format = "%s at %s:%i:\n";
const char* message_format = "\t%s\n\t%s\n";

void eltecg::detail::print_msg(eltecg::detail::MESSAGE_TYPE type, const char* file, int line, const char* msg, const char* expr)
{
#ifdef USE_COLOR_CODES
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, colors[static_cast<int>(type)] | W_F_I); //bold
#endif // USE_COLOR_CODES
	printf(file_pos_format, message_typenames[static_cast<int>(type)] , file, line);
#ifdef USE_COLOR_CODES
	SetConsoleTextAttribute(hstdout, colors[static_cast<int>(type)]);		//not bold
#endif // USE_COLOR_CODES
	printf(message_format, msg, expr);
#ifdef USE_COLOR_CODES
	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
#endif // USE_COLOR_CODES
}

bool eltecg::detail::gpu_check(const char * file, int line)
{
	GLenum err = glGetError();
	const char* msg; const char* expl;
	switch (err)
	{
	case GL_NO_ERROR: return true; break;	//returns true if ok!
	case GL_INVALID_ENUM:					msg = "Invalid enum";
		expl = "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_VALUE:					msg = "Invalid value";
		expl = "A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_OPERATION:				msg = "Invalid operation";
		expl = "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:	msg = "Invalid framebuffer operation";
		expl = "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_OUT_OF_MEMORY:					msg = "Out of memory";
		expl = "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
		break;
	case GL_STACK_UNDERFLOW:				msg = "Stack underflow";
		expl = "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
		break;
	case GL_STACK_OVERFLOW:					msg = "Stack overflow";
		expl = "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
		break;
	default:								msg = "Unknown error";
		expl = "glGetError() returned an unknown value. Weird.";
		break;
	}
#ifdef USE_COLOR_CODES
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, gpu_assert_col | W_F_I); //bold
#endif // USE_COLOR_CODES
	printf(file_pos_format, "OPENGL ERROR" , file, line);
#ifdef USE_COLOR_CODES
	SetConsoleTextAttribute(hstdout, gpu_assert_col);		//not bold
#endif // USE_COLOR_CODES
	printf(message_format, msg, expl);
#ifdef USE_COLOR_CODES
	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
#endif // USE_COLOR_CODES
	return false;
}

#endif