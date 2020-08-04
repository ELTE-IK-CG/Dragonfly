#pragma once
#include "BufferBase.h"
#include "../Traits/Range.h"
#include <string_view>
#include <iterator>

namespace df
{
	namespace detail
	{
		template<typename ItemType_>
		class ContiguousIterator	// Mostly boilerplate for G a pointer with optional safety checks
		{
#define _LOWER_ASSERT_			ASSERT(_ptr >= _begin, "df::detail::ContiguousIterator:  Iterator out of mapped range (below range).")
#define _UPPER_ASSERT_			ASSERT(_ptr <= _end,   "df::detail::ContiguousIterator: Iterator out of mapped range (too large).")
#define _RANGE_ASSERT_			_UPPER_RANGE_ASSERT_; _LOWER_RANGE_ASSERT_
#define _COMPATIBILITY_ASSERT_	ASSERT(_begin == other._begin && _end == other._end, "df::detail::ContiguousIterator: Incompatible iterators.");
			using iterator			= ContiguousIterator;
		public:
			using value_type		= ItemType_;
			using difference_type	= long;
			using pointer			= ItemType_ *;
			using reference			= ItemType_&;
			using iterator_category = std::contiguous_iterator_tag;
		private:
			pointer _ptr;
#ifdef _DEBUG
			const pointer _begin;
			const pointer _end;
#endif
		public:
#ifndef _DEBUG
			//ContiguousIterator(pointer ptr_) : _ptr(ptr_) {}
			ContiguousIterator(pointer ptr_, pointer range_begin_, pointer range_end_) : _ptr(ptr_) {}
#else
			ContiguousIterator(pointer ptr_, pointer range_begin_, pointer range_end_) : _ptr(ptr_), _begin(range_begin_), _end(range_end_) {}
#endif
			iterator& operator++() { ++ptr; _UPPER_ASSERT_; return *this; }
			iterator& operator--() { --ptr; _LOWER_ASSERT_; return *this; }
			iterator operator++(int) { iterator it = *this; ++(*this); return it; }
			iterator operator--(int) { iterator it = *this; --(*this); return it; }
			iterator& operator+=(difference_type n) { _ptr += n; _RANGE_ASSERT_; return *this; }
			iterator& operator-=(difference_type n) { _ptr -= n; _RANGE_ASSERT_; return *this; }
			iterator operator+(difference_type n) const { iterator it = *this; it += n; return it;}
			iterator operator-(difference_type n) const { iterator it = *this; it -= n; return it; }

				  reference operator[](difference_type n) { return *(*this + n); }
			const reference operator[](difference_type n) const { return *(*this + n); }

				  pointer operator ->() { return _ptr; }
			const pointer operator ->() const { return _ptr; }

				  reference operator*() { return _ptr; }
			const reference operator*() const { return _ptr; }

			difference_type operator-(iterator other) const { _COMPATIBILITY_ASSERT_; return _ptr - other._ptr; }

			bool operator==(const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr == other._ptr; }
			bool operator!=(const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr != other._ptr; }
			bool operator< (const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr < other._ptr; }
			bool operator> (const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr > other._ptr; }
			bool operator<=(const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr <= other._ptr; }
			bool operator>=(const iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr >= other._ptr; }
#undef _LOWER_ASSERT_
#undef _UPPER_ASSERT_
#undef _RANGE_ASSERT_
#undef _COMPATIBILITY_ASSERT_
		}; // class MappedPointerIterator

		template<typename TupleType_>
		class MappedBuffer
		{
			Buffer<TupleType_> _bufferView;
			TupleType_* _begin;
			TupleType_* _end;
			friend class Buffer<TupleType_>;
		public:
			//todo private + friend
			MappedBuffer(const Buffer<TupleType_> &buffer_)
				: _bufferView(buffer_.MakeView()),
				_begin(static_cast<TupleType_*>(glMapBuffer(static_cast<GLuint>(buffer_), GL_READ_WRITE))), _end(_begin + buffer_.Size())
			{}
			MappedBuffer(const Buffer<TupleType_>& buffer_, df::Range range)
				:_bufferView(buffer_.MakeView()),
				_begin(static_cast<TupleType_*>(glMapBufferRange(static_cast<GLuint>(buffer_), range.start * sizeof(TupleType_), range.num * sizeof(TupleType_), GL_READ_WRITE))),
				_end(_begin + range.num)
			{}
			MappedBuffer(const Mappedbuffer&) = delete;
			~MappedBuffer()	{ glUnmapBuffer(static_cast<GLuint>(_bufferView)); }


			ContiguousIterator begin() { return ContiguousIterator(_begin, _begin, _end); }
			const ContiguousIterator begin() const { return ContiguousIterator(_begin, _begin, _end); }
			ContiguousIterator end() { return ContiguousIterator(_begin, _end, _end); }
			const ContiguousIterator end() const { return ContiguousIterator(_begin, _end, _end); }
			const ContiguousIterator cbegin() const { return begin(); }
			const ContiguousIterator cend() const { return end(); }
		};
	}

	template <class ...ItemTypes_>
	class Buffer : public Buffer<std::tuple<ItemTypes_>>
	{
		//
	};

	template <class ItemType_>
	class Buffer<ItemType_> : public Buffer<std::tuple<ItemType_>>
	{
		using Item_Type = ItemType_;

		explicit Buffer(std::vector<Item_Type>&& vec)			: BufferBase(sizeof(Item_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
		explicit Buffer(const std::vector<Item_Type>& vec)		: BufferBase(sizeof(Item_Type)* vec.size(), static_cast<void*>(vec.begin())){}
		explicit Buffer(std::initializer_list<Item_Type> items) : BufferBase(sizeof(Item_Type)* items.size(), static_cast<void*>(items.begin())) {}
	};


	template <class ...ItemTypes_>
	class Buffer<std::tuple<ItemTypes_...>> : public BufferBase
	{
		Buffer(GLuint bufferId_, size_t bytes_) : BufferBase(bufferId_, bytes_) {}

	public:
		using Value_Type = std::tuple<ItemTypes_...>;
		using value_type = Value_Type;

		explicit Buffer(size_t alloc_size_)						 : BufferBase(sizeof(Value_Type)* alloc_size_, nullptr) {}
		explicit Buffer(std::vector<value_type>&& vec)			 : BufferBase(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
		explicit Buffer(const std::vector<value_type>& vec)		 : BufferBase(sizeof(Value_Type)* vec.size(), static_cast<void*>(vec.begin())) {}
		explicit Buffer(std::initializer_list<value_type> items) : BufferBase(sizeof(Value_Type)* items.size(), static_cast<void*>(items.begin())) {}

		Buffer(Buffer&& other_);
		Buffer(const Buffer&) = delete;
		Buffer& operator=(Buffer&& other_);
		Buffer& operator=(const Buffer&) = delete;
		
		size_t Size() const { return BufferBase::Bytes() / sizeof(Value_Type); }

		Buffer MakeView() const { return Buffer(this->_bufferId, this->_bytes); }

		detail::MappedBuffer operator[](df::Range range) { return detail::MappedBuffer(*this, range); }

	};


}