#include "BufferBase.h"

/*
df::BufferBase::BufferBase(GLuint bufferId_, size_t bytes_)
	: _bufferId(bufferId_), _bytes(bytes_)
{
	ASSERT(bufferInstances.count(this->_bufferId) != 0, "df::BufferBase: should have already had a buffer to copy");
	++bufferInstances[this->_bufferId];
}

df::BufferBase::BufferBase(int bytes_, void* data)
{
	glGenBuffers(1, &this->_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, this->_bufferId);
	glBufferStorage(GL_ARRAY_BUFFER, bytes_, data, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

	ASSERT(bufferInstances.count(this->_bufferId) == 0, "df::BufferBase: should not have any with this id.");

	bufferInstances[this->_bufferId] = 1;
	auto size = bufferInstances.size();
	this->_bytes = bytes_;
}

df::BufferBase::BufferBase(df::BufferBase &&other_) noexcept
	: _bufferId(std::exchange(other_._bufferId, 0)), _bytes(std::exchange(other_._bytes, 0))
{ 
}

df::BufferBase& df::BufferBase::operator=(df::BufferBase &&other_) noexcept
{
	if(this != &other_){
		std::swap(this->_bufferId, other_._bufferId);
		std::swap(this->_bytes, other_._bytes);
	}
	return *this;
}

df::BufferBase::~BufferBase()
{
	if (--bufferInstances[this->_bufferId] == 0)
		glDeleteBuffers(1, &this->_bufferId);
}
*/

inline df::detail::BufferLowLevelBase::BufferLowLevelBase(const BufferLowLevelBase& other_) : _id(other_._id), _bytes(other_._bytes), _flags(other_._flags)
{
	ASSERT(checkBufferFlag(_flags), "df::detail::BufferLowLevelBase: Invalid Flag setup.");
	ASSERT(_id != 0 && (_instances.count(_id) != 0 && _instances[_id] != 0), "df::detail::BufferLowLevelBase: Should already have an instance with this id.");
	++_instances[_id];
}

inline df::detail::BufferLowLevelBase::BufferLowLevelBase(size_t bytes_, BUFFER_BITS flags_, void* data) : _bytes(bytes_), _flags(flags_)
{
	ASSERT(checkBufferFlag(flags_), "df::detail::BufferLowLevelBase: Invalid Flag setup.");
	glGenBuffers(1, &_id);
	ASSERT(_id != 0 && (_instances.count(_id) == 0 || _instances[_id] == 0), "df::detail::BufferLowLevelBase: Should not have an instance already with this id.");
	glNamedBufferStorage(_id, _bytes, data, static_cast<GLenum>(flags_));
	_instances[_id] = 1;
}

inline df::detail::BufferLowLevelBase::~BufferLowLevelBase()
{
	if (_id != 0 && --_instances[_id] == 0)
		glDeleteBuffers(1, &_id);
}
