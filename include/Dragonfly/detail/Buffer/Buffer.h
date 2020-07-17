#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>

namespace df
{
	static std::map<GLuint, int> bufferInstances;

	namespace detail {
		template<typename NewType>
		struct ConvertVecStore {
			std::vector<NewType> vec;
		};
		template<typename NewType>
		struct ConvertVecRef {
			std::vector<NewType>& vec;
		};
	}

	template<typename NewType>
	detail::ConvertVecStore<NewType> Convert(std::vector<NewType>&& vec) {
		return ConvertVec(std::move(vec));
	}

	template<typename NewType>
	detail::ConvertVecRef<NewType> Convert(std::vector<NewType>& vec) {
		return ConvertVecRef(vec);
	}

	class BufferBase
	{
	protected:
		GLuint buffer_id = 0;

		// This MUST be called in each constructor, otherwise the bufferInstance counter
		// won't be increased and ~Buffer will have undefined behavior.
		void _initBuffer(int size, void* data)
		{
			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			glBufferStorage(GL_ARRAY_BUFFER, size, data, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

			bufferInstances[buffer_id] = 1;
		}

	public:
		~BufferBase()
		{
			if (--bufferInstances[buffer_id] == 0)
				glDeleteBuffers(1, &buffer_id);
		}
	};

	template <class ...ItemType>
	class Buffer : public BufferBase
	{		
	public:
		using value_type = std::tuple<ItemType...>;

		explicit Buffer(std::initializer_list<std::tuple<ItemType...>> items)
		{
			_initBuffer(_getParamsSize() * items.size(), (void*)items.begin());
		}

		explicit Buffer(std::vector<std::tuple<ItemType...>> vec)
		{
			_initBuffer(_getParamsSize() * vec.size(), vec);
		}

		template<typename NewType>
		explicit Buffer(const std::vector<NewType>& vec)
		{
			_initBuffer(_getParamsSize() * vec.size(), static_cast<void*>(vec));
		}

		template<typename ...NewTypes>
		explicit Buffer(const std::vector<detail::ConvertVecRef<std::tuple<NewTypes...>>>& vec)
		{
			_initBuffer(_getParamsSize() * vec.size(), vec);
		}

		Buffer(int alloc_size)
		{
			_initBuffer(_getParamsSize() * alloc_size, nullptr);
		}

		template<typename NewType>
		Buffer& operator=(const Buffer<NewType> buffer)
		{

			return buffer;
		}

		template <typename ...NewBufferType>
		Buffer<NewBufferType...> MakeView()
		{
			Buffer<NewBufferType...> newBuf();
			newBuf.buffer_id = buffer_id;
			// count instances
			return newBuf;
		}

	private:

		static constexpr int _getParamsSize()
		{
			return (sizeof(ItemType) + ...);
		}
	};
}