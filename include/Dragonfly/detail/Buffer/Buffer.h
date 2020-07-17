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
		void _initBuffer(int size, void* data);

	public:
		~BufferBase();
	};

	template <class ...ItemType>
	class Buffer : public BufferBase
	{
	public:
		using value_type = std::tuple<ItemType...>;

		explicit Buffer(std::initializer_list<std::tuple<ItemType...>> items);

		explicit Buffer(std::vector<std::tuple<ItemType...>> vec);

		template<typename NewType>
		explicit Buffer(const std::vector<NewType>& vec);

		template<typename ...NewTypes>
		explicit Buffer(const std::vector<detail::ConvertVecRef<std::tuple<NewTypes...>>>& vec);

		Buffer(int alloc_size);

		template<typename NewType>
		Buffer& operator=(const Buffer<NewType> buffer);

		template <typename ...NewBufferType>
		Buffer<NewBufferType...> MakeView();

	private:

		static constexpr int _getParamsSize()
		{
			return (sizeof(ItemType) + ...);
		}
	};
}