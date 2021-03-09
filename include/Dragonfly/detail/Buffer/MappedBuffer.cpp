#include "MappedBuffer.h"

using namespace df::detail;

void MappedBufferBase::_Map(GLintptr offset_, GLsizeiptr length_, df::MAP_BITS flags_)
{
	ASSERT(checkMapFlag(flags_), "Invalid mapping flags when using this buffer");
	ASSERT(checkValidBufferMapBit(flags_,_view.Flags()), "Inconsitent mapping flags with buffer object");
	ASSERT(offset_ + length_ <= _view.Bytes(), "Cannot map more then the buffer size.");
	ASSERT(!is_mapped, "Buffer is already mapped.");
	if (is_mapped) _Unmap();
	_begin = static_cast<char*>(glMapNamedBufferRange(_view.GetID(), offset_, length_, static_cast<GLbitfield>(flags_)));
	_end = _begin + length_;
	_flags = flags_;
	is_mapped = true;
}

void MappedBufferBase::_Flush(GLintptr offset_, GLsizeiptr length_)
{
	ASSERT(_flags && MAP_BITS::FLUSH, "MappedBuffer can only Flush if the MAP_BITS::FLUSH was used when Mapped");
	ASSERT(_begin + offset_ + length_ <= _end, "Cannot flush a range exceeding the end of the mapped buffer range.");
	ASSERT(!is_mapped, "Buffer must be mapped to Flush it.");
	if(is_mapped) glFlushMappedNamedBufferRange(_view.GetID(), offset_, length_);
}

void MappedBufferBase::_Unmap() {
	ASSERT(is_mapped, "Buffer was not mapped during Unmapping");
	if (is_mapped) glUnmapNamedBuffer(_view.GetID());
}