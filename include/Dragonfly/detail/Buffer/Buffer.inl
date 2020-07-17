#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"
#include <vector>

namespace df
{
	inline void BufferBase::_initBuffer(int size, void* data)
	{
		glGenBuffers(1, &this->buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer_id);
		glBufferStorage(GL_ARRAY_BUFFER, size, data, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

		bufferInstances[this->buffer_id] = 1;
	}

	inline BufferBase::~BufferBase()
	{
		if (--bufferInstances[this->buffer_id] == 0)
			glDeleteBuffers(1, &this->buffer_id);
	}

	template <class ...ItemType>
	explicit Buffer<ItemType...>::Buffer(std::initializer_list<std::tuple<ItemType...>> items)
	{
		_initBuffer(_getParamsSize() * items.size(), (void*)items.begin());
	}

	template <class ...ItemType>
	explicit Buffer<ItemType...>::Buffer(std::vector<std::tuple<ItemType...>> vec)
	{
		_initBuffer(_getParamsSize() * vec.size(), vec);
	}

	template <class ...ItemType>
	template <typename NewType>
	explicit Buffer<ItemType...>::Buffer(const std::vector<NewType>& vec)
	{
		_initBuffer(_getParamsSize() * vec.size(), static_cast<void*>(vec));
	}

	template <class ...ItemType>
	template <typename ...NewTypes>
	explicit Buffer<ItemType...>::Buffer(const std::vector<detail::ConvertVecRef<std::tuple<NewTypes...>>>& vec)
	{
		_initBuffer(_getParamsSize() * vec.size(), vec);
	}

	template <class ...ItemType>
	Buffer<ItemType...>::Buffer(int alloc_size)
	{
		_initBuffer(_getParamsSize() * alloc_size, nullptr);
	}

	template <class ...ItemType>
	template<typename NewType>
	Buffer<ItemType...>& Buffer<ItemType...>::operator=(const Buffer<NewType> buffer)
	{

		return buffer;
	}

	template <class ...ItemType>
	template <typename ...NewBufferType>
	Buffer<NewBufferType...> Buffer<ItemType...>::MakeView()
	{
		Buffer<NewBufferType...> newBuf();
		newBuf.buffer_id = buffer_id;
		// count instances
		return newBuf;
	}
}