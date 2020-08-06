#pragma once
#include "BufferBase.h"

namespace df{

enum class MAP_BITS : GLenum
{
	NONE		= GL_NONE,						// Zero
	READ		= GL_MAP_READ_BIT,				// Only use this with df::Buffer
	WRITE		= GL_MAP_WRITE_BIT,				// Automatically set
	PERSISTENT	= GL_MAP_PERSISTENT_BIT,		// Cannot use with StorageBuffer
	COHERENT	= GL_MAP_COHERENT_BIT,			// Only if persistent
	INVALIDATE	= GL_MAP_INVALIDATE_RANGE_BIT,	// Only for overwriting (no read but write)
	FLUSH		= GL_MAP_FLUSH_EXPLICIT_BIT,	// Discrete subranges may be modified and flushed (write bit must be set)
	DONT_SYNC	= GL_MAP_UNSYNCHRONIZED_BIT,	// Does not sync when mapping
};  ENUM_CLASS_FLAG_OPERATORS(MAP_BITS)


namespace detail {

constexpr bool checkMapFlag(MAP_BITS flags) {
	return  checkBufferFlag(static_cast<BUFFER_BITS>(flags))
		&& ((flags && (~MAP_BITS::READ & MAP_BITS::WRITE)) || !(flags && MAP_BITS::INVALIDATE))
		&& ((flags && MAP_BITS::WRITE) || !(flags && MAP_BITS::FLUSH));
}

template<typename ValueType_>
class MappedBuffer
{
	BufferLowLevelBase _view;
	ValueType_* _begin, * _end;
	MAP_BITS _flags;
	bool is_mapped;
public:
	MappedBuffer(const BufferLowLevelBase &base_, Range range_, MAP_BITS flags_) : _view(base_), _flags(flags_), is_mapped(false){
		
	}
};


} // namespace detail
} // namespace df

/*
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
*/
#include "Dragonfly/detail/Buffer/MappedBuffer.inl"