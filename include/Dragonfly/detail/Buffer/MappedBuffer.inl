#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"
#include <vector>

namespace df
{
/*
template<typename TupleType_>
inline detail::MappedBuffer<TupleType_>::MappedBuffer(const Buffer<TupleType_>& buffer_, df::Range range)
	:_bufferView(buffer_.MakeView()) //_begin(nullptr), _end(nullptr)
{
	size_t length = (range.num > _bufferView.Size() - range.start ? _bufferView.Size() - range.start : range.num);
	GLuint id = static_cast<GLuint>(_bufferView);
	GL_CHECK;
	void* raw_ptr = glMapNamedBufferRange(id, range.start * sizeof(TupleType_), length * sizeof(TupleType_), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	GL_CHECK;
	ASSERT(raw_ptr != nullptr, "df::detail::MappedBuffer: glMapBuffer returned a nullptr");
	_begin = static_cast<TupleType_*>(raw_ptr);
	_end = _begin + length;
}*/
	   	 
}