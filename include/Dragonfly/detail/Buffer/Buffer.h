#pragma once
#include "../Traits/Range.h"
#include "BufferBase.h"
#include "MappedBuffer.h"


namespace df {

template<typename ...ItemTypes_>
class StorageBuffer : public detail::BufferBase<ItemTypes_...> // no GL_MAP_READ_BIT and no GL_MAP_WRITE_BIT is set
{
protected:
	using Base = detail::BufferBase<ItemTypes_...>;
	using Base::Single_Type, Base::Tuple_Type;
	constexpr BUFFER_BITS _RequiredFlag = BUFFER_BITS::NONE;
	constexpr MAP_BITS _DefaultMapFlags = MAP_BITS::NONE;
protected:
	// for creating a view
	StorageBuffer(const detail::BufferLowLevelBase& base_) : Base::Base(base_) {}
public:
	StorageBuffer(size_t len_,								BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(len_, flags_ | _RequiredFlag){}
	StorageBuffer(const std::vector<Single_Type>& vec_,		BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	StorageBuffer(std::initializer_list<Single_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	StorageBuffer(const std::vector<Tuple_Type>& vec_,		BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	StorageBuffer(std::initializer_list<Tuple_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}

	template<typename ...NewItemTypes>
	StorageBuffer<NewItemTypes...> MakeView() const { return StorageBuffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
};

template<typename ...ItemTypes_>
class ReadBuffer : public StorageBuffer<ItemTypes_...> // the GL_MAP_READ_BIT is set, but no GL_MAP_WRITE_BIT is set
{
protected:
	using Base = StorageBuffer<ItemTypes_...>;
	using Base::Single_Type, Base::Tuple_Type;
	constexpr BUFFER_BITS _RequiredFlag = Base::_RequiredFlag | BUFFER_BITS::READ;
	constexpr MAP_BITS _DefaultMapFlags = Base::_DefaultMapFlags | MAP_BITS::READ;
protected:
	ReadBuffer(const detail::BufferLowLevelBase& base_) : Base::Base(base_) {}
public:
	ReadBuffer(size_t len_,								BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(len_, flags_ | _RequiredFlag) {}
	ReadBuffer(const std::vector<Single_Type>& vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	ReadBuffer(std::initializer_list<Single_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	ReadBuffer(const std::vector<Tuple_Type>& vec_,		BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	ReadBuffer(std::initializer_list<Tuple_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}

	template<typename ...NewItemTypes>
	ReadBuffer<NewItemTypes...> MakeView() const { return ReadBuffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }

	const detail::MappedBuffer<ItemTypes_> MapBuffer(Range range_, MAP_BITS flags_ = _DefaultMapFlags) const;
	const detail::MappedBuffer<ItemTypes_> operator[](Range range_) const{ return MapBuffer(range_); }
};

template<typename ...ItemTypes_>
class Buffer: public ReadBuffer<ItemTypes_...> // the GL_MAP_WRITE_BIT is set
{
	template<typename> friend class Buffer;
protected:
	using Base = ReadBuffer<ItemTypes_...>;
	using Base::Single_Type, Base::Tuple_Type;
	constexpr BUFFER_BITS _RequiredFlag = Base::_RequiredFlag | BUFFER_BITS::WRITE;
	constexpr MAP_BITS _DefaultMapFlags = Base::_DefaultMapFlags | MAP_BITS::WRITE;
protected:
	Buffer(const detail::BufferLowLevelBase& base_) : Base::Base(base_) {}
public:
	Buffer(size_t len_,								BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(len_, flags_ | _RequiredFlag) {}
	Buffer(const std::vector<Single_Type>& vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	Buffer(std::initializer_list<Single_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	Buffer(const std::vector<Tuple_Type>& vec_,		BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}
	Buffer(std::initializer_list<Tuple_Type> vec_,	BUFFER_BITS flags_ = _RequiredFlag) : Base::Base(vec_, flags_ | _RequiredFlag){}

	template<typename ...NewItemTypes>
	Buffer<NewItemTypes...> MakeView() const { return Buffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
	Buffer MakeView() const { Buffer(static_cast<detail::BufferLowLevelBase&>(*this)); } //doesitwork???

	detail::MappedBuffer<ItemTypes_> MapBuffer(Range range_, MAP_BITS flags_ = _DefaultMapFlags);
	detail::MappedBuffer<ItemTypes_> operator[](Range range_) { return MapBuffer(range_); }
};

} //namespace df

/*
namespace df
{
namespace detail{ template<typename TupleType_> class MappedBuffer;}

template <class ...ItemTypes_>
class Buffer : public Buffer<std::tuple<ItemTypes_...>>
{
protected:
	using Value_Type = std::tuple<ItemTypes_...>;
	using value_type = Value_Type;
	using Base = Buffer<std::tuple<ItemTypes_...>>;
public:	
	explicit Buffer(std::vector<Value_Type>&& vec) : Base(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	explicit Buffer(const std::vector<Value_Type>& vec) : Base(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	Buffer(std::initializer_list<Value_Type> items) : Base(sizeof(Value_Type)* items.size(), (void*)(items.begin())) {}
};

template <class ItemType_>
class Buffer<ItemType_> : public Buffer<std::tuple<ItemType_>>
{
private:
	using Base = Buffer<std::tuple<ItemType_>>;
	using Item_Type = ItemType_;
public:
	using Base::Base;

	explicit Buffer(std::vector<Item_Type>&& vec) : Base(sizeof(Item_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	explicit Buffer(const std::vector<Item_Type>& vec) : Base(sizeof(Item_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	Buffer(std::initializer_list<Item_Type> items) : Base(sizeof(Item_Type)* items.size(), (void*)(items.begin())) {}

	detail::MappedBuffer<Item_Type> operator[](df::Range range) { return detail::MappedBuffer<Item_Type>(*this, range); }
	Buffer MakeView() const { return Buffer(this->_bufferId, this->_bytes); }

	detail::MappedBuffer<Item_Type> Map(Range range, GLenum flags);
	detail::MappedBuffer<Item_Type> Read(Range range);
	detail::MappedBuffer<Item_Type> Write(Range range);
};


template <class ...ItemTypes_>
class Buffer<std::tuple<ItemTypes_...>> : public BufferBase
{
protected:
	Buffer(GLuint bufferId_, size_t bytes_) : BufferBase(bufferId_, bytes_) {}
	Buffer(int bytes_, void* data_) : BufferBase(bytes_, data_) {}
public:
	using Value_Type = std::tuple<ItemTypes_...>;
	using value_type = Value_Type;

	Buffer(size_t alloc_size_) : BufferBase(sizeof(Value_Type)* alloc_size_, nullptr) {}
	explicit Buffer(std::vector<value_type>&& vec) : BufferBase(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	explicit Buffer(const std::vector<value_type>& vec) : BufferBase(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
	Buffer(std::initializer_list<value_type> items) : BufferBase(sizeof(Value_Type)* items.size(), (void*)(items.begin())) {}

	using BufferBase::BufferBase;
	using BufferBase::operator=;

	Buffer(Buffer&& other_);
	Buffer(const Buffer&) = delete;
	Buffer& operator=(Buffer&& other_);
	Buffer& operator=(const Buffer&) = delete;
	
	size_t Size() const { return BufferBase::Bytes() / sizeof(Value_Type); }

	Buffer MakeView() const { return Buffer(this->_bufferId, this->_bytes); }

	detail::MappedBuffer<Value_Type> operator[](df::Range range) { return detail::MappedBuffer<Value_Type>(*this, range); }

};
} //namespace std

#include "Dragonfly/detail/Buffer/Buffer.inl"

*/