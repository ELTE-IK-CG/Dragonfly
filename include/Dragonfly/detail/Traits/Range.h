#pragma once
#include <gl\glew.h>
#include <iterator>

namespace df
{
static const GLuint All = -1;

struct Range
{
	GLuint start, num;

	constexpr explicit Range(GLuint start_ = 0, GLuint num_ = All) : start(start_), num(num_) {}
	constexpr Range operator>> (GLuint num_) const { return Range{ this->start, num_ }; }
};


namespace detail
{
#define _LOWER_ASSERT_			ASSERT(this->_ptr >= this->_begin, "df::detail::ContiguousIterator:  Iterator out of mapped range (below range).")
#define _UPPER_ASSERT_			ASSERT(this->_ptr <= this->_end,   "df::detail::ContiguousIterator: Iterator out of mapped range (too large).")
#define _RANGE_ASSERT_			_UPPER_ASSERT_; _LOWER_ASSERT_
#define _COMPATIBILITY_ASSERT_	ASSERT(this->_begin == other._begin && this->_end == other._end, "df::detail::ContiguousIterator: Incompatible iterators.");

template<typename ItemType_>
class ContiguousConstIterator	// Mostly boilerplate for a pointer with optional safety checks
{
private:
	using const_iterator = ContiguousConstIterator;
public:
	using iterator_category	= std::random_access_iterator_tag;
	using value_type		= ItemType_;
	using difference_type	= long;
	using pointer			= ItemType_*;
	using reference			= const ItemType_&;
protected:
	pointer _ptr;
#ifdef _DEBUG
	const pointer _begin;
	const pointer _end;
#endif
public:
#ifndef _DEBUG
	//ContiguousConstIterator(pointer ptr_) : _ptr(ptr_) {}
	ContiguousConstIterator(pointer ptr_, pointer range_begin_, pointer range_end_) : _ptr(ptr_) {}
#else
	ContiguousConstIterator(pointer ptr_, pointer range_begin_, pointer range_end_) : _ptr(ptr_), _begin(range_begin_), _end(range_end_) {}
#endif
	const_iterator& operator++() { ++_ptr; _UPPER_ASSERT_; return *this; }
	const_iterator& operator--() { --_ptr; _LOWER_ASSERT_; return *this; }
	const_iterator operator++(int) { const_iterator it = *this; ++(*this); return it; }
	const_iterator operator--(int) { const_iterator it = *this; --(*this); return it; }
	const_iterator& operator+=(difference_type n) { _ptr += n; _RANGE_ASSERT_; return *this; }
	const_iterator& operator-=(difference_type n) { _ptr -= n; _RANGE_ASSERT_; return *this; }
	const_iterator operator+(difference_type n) const { const_iterator it = *this; it += n; return it; }
	const_iterator operator-(difference_type n) const { const_iterator it = *this; it -= n; return it; }

	reference operator[](difference_type n) { return *(*this + n); }
	const reference operator[](difference_type n) const { return *(*this + n); }

	pointer operator ->() { return _ptr; }
	const pointer operator ->() const { return _ptr; }

	reference operator*() { return *_ptr; }
	const reference operator*() const { return *_ptr; }

	difference_type operator-(const_iterator other) const { _COMPATIBILITY_ASSERT_; return _ptr - other._ptr; }

	bool operator==(const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr == other._ptr; }
	bool operator!=(const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr != other._ptr; }
	bool operator< (const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr < other._ptr; }
	bool operator> (const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr > other._ptr; }
	bool operator<=(const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr <= other._ptr; }
	bool operator>=(const const_iterator& other) const { _COMPATIBILITY_ASSERT_; return _ptr >= other._ptr; }
}; // class MappedPointerIterator

template<typename ItemType_>
class ContiguousIterator : public ContiguousConstIterator<ItemType_>
{
private:
	using Base = ContiguousConstIterator<ItemType_>;
	using iterator = ContiguousIterator;
	using const_iterator = typename Base::const_iterator;
public:
	using iterator_category = typename Base::iterator_category;
	using value_type		= typename Base::value_type;
	using difference_type	= typename Base::difference_type;
	using pointer			= typename Base::pointer;
	using reference			= ItemType_&;

	using Base::Base;

	iterator& operator++() { ++this->_ptr; _UPPER_ASSERT_; return *this; }
	iterator& operator--() { --this->_ptr; _LOWER_ASSERT_; return *this; }
	iterator operator++(int) { iterator it = *this; ++(*this); return it; }
	iterator operator--(int) { iterator it = *this; --(*this); return it; }
	iterator& operator+=(difference_type n) { this->_ptr += n; _RANGE_ASSERT_; return *this; }
	iterator& operator-=(difference_type n) { this->_ptr -= n; _RANGE_ASSERT_; return *this; }
	iterator operator+(difference_type n) const { iterator it = *this; it += n; return it; }
	iterator operator-(difference_type n) const { iterator it = *this; it -= n; return it; }

	reference operator[](difference_type n) { return *(*this + n); }
	const reference operator[](difference_type n) const { return *(*this + n); }

	pointer operator ->() { return this->_ptr; }
	const pointer operator ->() const { return this->_ptr; }

	reference operator*() { return *this->_ptr; }
	const reference operator*() const { return *this->_ptr; }
};
#undef _LOWER_ASSERT_
#undef _UPPER_ASSERT_
#undef _RANGE_ASSERT_
#undef _COMPATIBILITY_ASSERT_

}} // namespace df::detail