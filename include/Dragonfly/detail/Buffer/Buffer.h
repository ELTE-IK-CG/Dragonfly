#pragma once
#include "BufferBase.h"

namespace df
{
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