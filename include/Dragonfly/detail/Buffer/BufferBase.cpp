#include "BufferBase.h"

df::BufferBase::BufferBase(int size, void* data)
{
	glGenBuffers(1, &this->buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer_id);
	glBufferStorage(GL_ARRAY_BUFFER, size, data, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
	
	bufferInstances[this->buffer_id] = 1;
	this->size = size;
}

df::BufferBase::BufferBase(df::BufferBase &&other_) noexcept
	: buffer_id(std::exchange(other_.buffer_id, 0)), size(std::exchange(other_.size, 0))
{ 
}

df::BufferBase& df::BufferBase::operator=(df::BufferBase &&other_)
{
	if(this != &other_){
		std::swap(this->buffer_id, other_.buffer_id);
		std::swap(this->size, other_.size);
	}
	return *this;
}

df::BufferBase::~BufferBase()
{
	if (--bufferInstances[this->buffer_id] == 0)
		glDeleteBuffers(1, &this->buffer_id);
}
