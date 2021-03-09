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
	using Base::Single_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = BUFFER_BITS::NONE;
	static constexpr MAP_BITS _defaultMapFlags = MAP_BITS::NONE;
protected:
	// for creating a view
	explicit StorageBuffer(const detail::BufferLowLevelBase& base_) : Base::BufferBase(base_) {}
public:
	explicit StorageBuffer(size_t len_,						BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(len_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<Single_Type>&vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<Pair_Type>  &vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(const std::vector		<Tuple_Type> &vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(std::initializer_list	<Single_Type> vec_,	BUFFER_BITS flags_ = _requiredFlag)	: Base::BufferBase(vec_, flags_ | _requiredFlag){}
	StorageBuffer(std::initializer_list	<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::BufferBase(vec_, flags_ | _requiredFlag){}

	template<typename ...NewItemTypes_>
	StorageBuffer<NewItemTypes_...> MakeView() const { return StorageBuffer<NewItemTypes_...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
};


template<typename ...ItemTypes_>
class ReadBuffer : public StorageBuffer<ItemTypes_...> // the GL_MAP_READ_BIT is set, but no GL_MAP_WRITE_BIT is set
{
protected:
	using Base = StorageBuffer<ItemTypes_...>;
	using Base::Single_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = BUFFER_BITS::READ;//Base::_requiredFlag | BUFFER_BITS::READ;
	static constexpr MAP_BITS _defaultMapFlags = MAP_BITS::READ;//Base::_defaultMapFlags | MAP_BITS::READ;
protected:
	explicit ReadBuffer(const detail::BufferLowLevelBase& base_) : Base::StorageBuffer(base_) {}
public:
	explicit ReadBuffer(size_t len_,					BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(len_, flags_ | _requiredFlag) {}
	ReadBuffer(const std::vector	<Single_Type>&vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(const std::vector	<Pair_Type>  &vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(const std::vector	<Tuple_Type> &vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(std::initializer_list<Single_Type> vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}
	ReadBuffer(std::initializer_list<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::StorageBuffer(vec_, flags_ | _requiredFlag){}

	template<typename ...NewItemTypes_>
	[[nodiscard]] ReadBuffer<NewItemTypes_...> MakeView() const { return ReadBuffer<NewItemTypes_...>(static_cast<detail::BufferLowLevelBase&>(*this)); }

	[[nodiscard]] operator std::vector<Single_Type>()const { return this->template _DownloadData<Single_Type>(); }
	[[nodiscard]] operator std::vector<Pair_Type>()  const { return this->template _DownloadData<Pair_Type>(); }
	[[nodiscard]] operator std::vector<Tuple_Type>() const { return this->template _DownloadData<Tuple_Type>(); }

	const detail::MappedBuffer<Tuple_Type> Map(Range range_ = Range(), MAP_BITS flags_ = _defaultMapFlags) const {return detail::MappedBuffer<Tuple_Type>(*this, range_, flags_);}
};

template<typename ...ItemTypes_>
class Buffer: public ReadBuffer<ItemTypes_...> // the GL_MAP_WRITE_BIT is set
{
	//template<typename> friend class Buffer<>;
protected:
	using Base = ReadBuffer<ItemTypes_...>;
	using Base::Single_Type, Base::Pair_Type, Base::Tuple_Type;
	static constexpr BUFFER_BITS _requiredFlag = static_cast<BUFFER_BITS>(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);//Base::_requiredFlag | BUFFER_BITS::WRITE;
	static constexpr MAP_BITS _defaultMapFlags = static_cast<MAP_BITS>(GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);//Base::_defaultMapFlags | MAP_BITS::WRITE;
protected:
	explicit Buffer(const detail::BufferLowLevelBase& base_) : Base::ReadBuffer(base_) {}
public:
	explicit Buffer(size_t len_,					BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(len_, flags_ | _requiredFlag){}
	Buffer(const std::vector	<Single_Type>&vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(const std::vector	<Pair_Type>  &vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(const std::vector	<Tuple_Type> &vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(std::initializer_list<Single_Type> vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}
	Buffer(std::initializer_list<Tuple_Type>  vec_,	BUFFER_BITS flags_ = _requiredFlag) : Base::ReadBuffer(vec_, flags_ | _requiredFlag){}// no Pair_Type to avoid ambiguity

	template<typename ...NewItemTypes_>
	[[nodiscard]] Buffer<NewItemTypes_...> MakeView() const { return Buffer<NewItemTypes_...>(static_cast<detail::BufferLowLevelBase&>(*this)); }
	[[nodiscard]] Buffer MakeView() const { return Buffer(static_cast<detail::BufferLowLevelBase&>(*this)); } //Does it work???

	using Base::Map; // const version
	detail::MappedBuffer<Tuple_Type> Map(Range range_ = Range(), MAP_BITS flags_ = _defaultMapFlags) { return detail::MappedBuffer<Tuple_Type>(*this, range_, flags_); }

	Buffer& operator=(const std::vector<Single_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(Single_Type) * vec.size()); return *this; }
	Buffer& operator=(const std::vector<Pair_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(Pair_Type) * vec.size()); return *this; }
	Buffer& operator=(const std::vector<Tuple_Type>& vec) { this->_UploadData(static_cast<const void*>(vec.data()), sizeof(Tuple_Type) * vec.size()); return *this; }
};

} //namespace df
