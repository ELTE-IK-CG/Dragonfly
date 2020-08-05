#pragma once
#include "BufferBase.h"
#include "../Traits/Range.h"
#include "MappedBuffer.h"

namespace df
{
namespace detail{ template<typename TupleType_> class MappedBuffer;}

template <class ...ItemTypes_>
class Buffer : public Buffer<std::tuple<ItemTypes_...>>
{
private:
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
/** /
	Buffer(Buffer&& other_);
	Buffer(const Buffer&) = delete;
	Buffer& operator=(Buffer&& other_);
	Buffer& operator=(const Buffer&) = delete;
	/**/
	size_t Size() const { return BufferBase::Bytes() / sizeof(Value_Type); }

	Buffer MakeView() const { return Buffer(this->_bufferId, this->_bytes); }

	detail::MappedBuffer<Value_Type> operator[](df::Range range) { return detail::MappedBuffer<Value_Type>(*this, range); }

};
} //namespace std

#include "Dragonfly/detail/Buffer/Buffer.inl"