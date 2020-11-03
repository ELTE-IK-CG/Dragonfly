#pragma once
#include "BufferBase.h"
#include "../Traits/Range.h"

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

} //namespace df

namespace df::detail{

template<MAP_BITS flags_> constexpr bool checkMapFlag() {
	return  checkBufferFlag<static_cast<BUFFER_BITS>(flags_)>()
		&& ((flags_ && (~MAP_BITS::READ & MAP_BITS::WRITE)) || !(flags_ && MAP_BITS::INVALIDATE))
		&& ((flags_ && MAP_BITS::WRITE) || !(flags_ && MAP_BITS::FLUSH));
}
inline bool checkMapFlag(MAP_BITS flags_) {
	return  checkBufferFlag(static_cast<BUFFER_BITS>(flags_))
		&& ((flags_ && (~MAP_BITS::READ & MAP_BITS::WRITE)) || !(flags_ && MAP_BITS::INVALIDATE))
		&& ((flags_ && MAP_BITS::WRITE) || !(flags_ && MAP_BITS::FLUSH));
}
template<MAP_BITS mapflags_, BUFFER_BITS bufflags_>
constexpr bool checkValidBufferMapBit() {
	constexpr BUFFER_BITS mapflags = static_cast<BUFFER_BITS>(mapflags_);
	return (~bufflags_ & BUFFER_BITS::ALL & mapflags) == BUFFER_BITS::NONE;
}
inline bool checkValidBufferMapBit(MAP_BITS mapflags_, BUFFER_BITS bufflags_) {
	BUFFER_BITS mapflags = static_cast<BUFFER_BITS>(mapflags_);
	return (~bufflags_ & BUFFER_BITS::ALL & mapflags) == BUFFER_BITS::NONE;
}

class MappedBufferBase
{
public:
	~MappedBufferBase() { if (is_mapped) _Unmap(); }
	size_t Bytes() const { return is_mapped ? _end - _begin : 0; }
	void* Data() const { return is_mapped ? _begin : nullptr; }

protected:
	//todo move or copy view_? 
	MappedBufferBase(BufferLowLevelBase&& view_) : _begin(nullptr), _end(nullptr), _view(std::move(view_)), _flags(MAP_BITS::NONE), is_mapped(false) {}
	MappedBufferBase(const MappedBufferBase&) = delete;

	void _Map(GLintptr offset_, GLsizeiptr length_, MAP_BITS flags_);
	void _Flush(GLintptr offset_, GLsizeiptr length_);
	void _Unmap();

protected:
	char *_begin, *_end;
	BufferLowLevelBase _view;
private:
	MAP_BITS _flags;
	bool is_mapped;
};


template<typename ValueType_>
class MappedBuffer : public MappedBufferBase
{
	static constexpr size_t valueSize = sizeof(ValueType_);
	using Base = MappedBufferBase;
	using iterator = df::detail::ContiguousIterator<ValueType_>;
	using const_iterator = df::detail::ContiguousConstIterator<ValueType_>;
	//using reverse_iterator = todo
	//using const_reverse_iterator = todo
public:
	MappedBuffer(BufferLowLevelBase view_, Range range_, MAP_BITS flags_) : Base(std::move(view_)){ Map(range_, flags_); }
	~MappedBuffer() {}
	MappedBuffer(const MappedBuffer&) = delete;

	void Map(Range range_, MAP_BITS flags_) { _Map(range_.start * valueSize, range_.num == df::All ? _view.Bytes() - range_.start* valueSize : range_.num * valueSize, flags_); }
	void Unmap(){ _Unmap();}
	void Flush(Range range_ = Range()) { _Flush(range_.start * valueSize, range_.num == df::All ? _view.Bytes() - range_.start * valueSize : range_.num * valueSize);}

	iterator		begin()			{ return iterator(		reinterpret_cast<ValueType_*>(_begin),reinterpret_cast<ValueType_*>(_begin), reinterpret_cast<ValueType_*>(_end)); }
	const_iterator	begin()  const	{ return const_iterator(reinterpret_cast<ValueType_*>(_begin),reinterpret_cast<ValueType_*>(_begin), reinterpret_cast<ValueType_*>(_end)); }
	iterator		end()			{ return iterator(		reinterpret_cast<ValueType_*>(_end),  reinterpret_cast<ValueType_*>(_begin), reinterpret_cast<ValueType_*>(_end)); }
	const_iterator	end()	 const	{ return const_iterator(reinterpret_cast<ValueType_*>(_end),  reinterpret_cast<ValueType_*>(_begin), reinterpret_cast<ValueType_*>(_end)); }
	const_iterator	cbegin() const	{ return begin(); }
	const_iterator	cend()	 const	{ return end(); }

	size_t size() const { return Bytes() * valueSize; }
};

} //namespace df::detail