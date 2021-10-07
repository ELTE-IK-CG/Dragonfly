#include "UniformBuffer.h"

bool df::UniformBlockLayout::operator==(const UniformBlockLayout& other_) const
{
	return other_.byte_size == byte_size && other_.uniforms == uniforms; // each variable is named the same and has the same type
}