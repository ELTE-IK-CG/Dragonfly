#pragma once
#include <gl\glew.h>

namespace df
{
	static const GLuint All = -1;

	struct Range
	{
		GLuint start, num;

		Range(GLuint start_, GLuint num_ = All) : start(start_), num(num_){}
		constexpr Range operator>> (GLuint num_) { return Range{ this->start, num_ }; }
	};

}