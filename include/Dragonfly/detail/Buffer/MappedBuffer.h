#pragma once
#include "Buffer.h"

namespace df{

template <class ...ItemTypes_> class Buffer;

namespace detail{

template<typename TupleType_>
class MappedBuffer
{
private:
	Buffer<TupleType_> _bufferView;
	TupleType_* _begin;
	TupleType_* _end;
	friend class Buffer<TupleType_>;
	using iterator = ContiguousIterator<TupleType_>;
	using const_iterator = ContiguousConstIterator<TupleType_>;
	MappedBuffer(const Buffer<TupleType_>& buffer_)
		: _bufferView(buffer_.MakeView()),
		_begin(static_cast<TupleType_*>(glMapNamedBuffer(static_cast<GLuint>(buffer_), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT))), _end(_begin + buffer_.Size())
	{}
	MappedBuffer(const Buffer<TupleType_>& buffer_, df::Range range);
	MappedBuffer(const MappedBuffer<TupleType_>&) = delete;
public:
	~MappedBuffer() { glUnmapNamedBuffer(static_cast<GLuint>(_bufferView)); }
	
	iterator		begin() { return iterator(_begin, _begin, _end); }
	const_iterator	begin() const { return const_iterator(_begin, _begin, _end); }
	iterator		end() { return iterator(_end, _begin, _end); }
	const_iterator	end() const { return const_iterator(_end, _begin, _end); }
	const_iterator	cbegin() const { return begin(); }
	const_iterator	cend() const { return end(); }
};

}} //namespace df::detail

#include "Dragonfly/detail/Buffer/MappedBuffer.inl"