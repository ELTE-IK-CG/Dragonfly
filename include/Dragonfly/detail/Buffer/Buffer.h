#pragma once
#include "../Traits/Range.h"
#include "BufferBase.h"
#include "MappedBuffer.h"
#include <type_traits>

namespace df {

template<typename ...ItemTypes_>
class StorageBuffer : public detail::BufferBase<ItemTypes_...> // no GL_MAP_READ_BIT and no GL_MAP_WRITE_BIT is set
{
protected:
	using Base = detail::BufferBase<ItemTypes_...>;
	using Base::First_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = BUFFER_BITS::NONE;
	static constexpr MAP_BITS _defaultMapFlags = MAP_BITS::NONE;
protected:
	// for creating a view
	explicit StorageBuffer(const detail::BufferLowLevelBase& base_) : Base::BufferBase(base_) {}
public:
	explicit StorageBuffer(size_t len_,						BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(len_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<First_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(std::initializer_list	<First_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<Pair_Type>&  vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(std::initializer_list	<Pair_Type>   vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<Tuple_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(std::initializer_list	<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(vec_, flags_ | _requiredFlag){}

	template<typename ...NewItemTypes>
	StorageBuffer<NewItemTypes...> MakeView() const { return StorageBuffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
};


template<typename ...ItemTypes_>
class ReadBuffer : public StorageBuffer<ItemTypes_...> // the GL_MAP_READ_BIT is set, but no GL_MAP_WRITE_BIT is set
{
protected:
	using Base = StorageBuffer<ItemTypes_...>;
	using Base::First_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = BUFFER_BITS::READ;//Base::_requiredFlag | BUFFER_BITS::READ;
	static constexpr MAP_BITS _defaultMapFlags = MAP_BITS::READ;//Base::_defaultMapFlags | MAP_BITS::READ;
protected:
	ReadBuffer(const detail::BufferLowLevelBase& base_) : Base::StorageBuffer(base_) {}
public:
	explicit ReadBuffer(size_t len_,					BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(len_, flags_ | _requiredFlag) {}
	ReadBuffer(const std::vector	<First_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(std::initializer_list<First_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(const std::vector	<Pair_Type>&  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	//ReadBuffer(std::initializer_list<Pair_Type >  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(const std::vector	<Tuple_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(std::initializer_list<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}

	template<typename ...NewItemTypes>
	ReadBuffer<NewItemTypes...> MakeView() const { return ReadBuffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }

	[[nodiscard]] operator std::vector<First_Type>() const { return this->_DownloadData<First_Type>(); }
	[[nodiscard]] operator std::vector<Pair_Type>()  const { return this->_DownloadData<Pair_Type>(); }
	[[nodiscard]] operator std::vector<Tuple_Type>() const { return this->_DownloadData<Tuple_Type>(); }

	//const detail::MappedBuffer<ItemTypes_> MapBuffer(Range range_, MAP_BITS flags_ = _defaultMapFlags) const;
	//const detail::MappedBuffer<ItemTypes_> operator[](Range range_) const{ return MapBuffer(range_); }
};

template<typename ...ItemTypes_>
class Buffer: public ReadBuffer<ItemTypes_...> // the GL_MAP_WRITE_BIT is set
{
	//template<typename> friend class Buffer<>;
protected:
	using Base = ReadBuffer<ItemTypes_...>;
	using Base::First_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = static_cast<BUFFER_BITS>(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);//Base::_requiredFlag | BUFFER_BITS::WRITE;
	static constexpr MAP_BITS _defaultMapFlags = static_cast<MAP_BITS>(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);//Base::_defaultMapFlags | MAP_BITS::WRITE;
protected:
	Buffer(const detail::BufferLowLevelBase& base_) : Base::ReadBuffer(base_) {}
public:
	explicit Buffer(size_t len_,					BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(len_, flags_ | _requiredFlag) {}
	Buffer(const std::vector	<First_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(std::initializer_list<First_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(const std::vector	<Pair_Type>&  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	//Buffer(std::initializer_list<Pair_Type>   vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(const std::vector	<Tuple_Type>& vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(std::initializer_list<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}

	template<typename ...NewItemTypes>
	Buffer<NewItemTypes...> MakeView() const { return Buffer<NewItemTypes...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
	Buffer MakeView() const { return Buffer(static_cast<detail::BufferLowLevelBase&>(*this)); } //doesitwork???

	//detail::MappedBuffer<ItemTypes_> MapBuffer(Range range_, MAP_BITS flags_ = _defaultMapFlags);
	//detail::MappedBuffer<ItemTypes_> operator[](Range range_) { return MapBuffer(range_); }
	Buffer operator=(const std::vector<First_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(First_Type) * vec.size()); return *this; }
	Buffer operator=(const std::vector<Pair_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(Pair_Type) * vec.size()); return *this; }
	Buffer operator=(const std::vector<Tuple_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(Tuple_Type) * vec.size()); return *this; }
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