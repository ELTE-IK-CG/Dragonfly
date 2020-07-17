#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"
#include "Dragonfly/detail/Vao/NewVao.h"
#include "Dragonfly/detail/Traits/BufferInternalFormats.h"

namespace df
{
	template <typename T1>
	NewVao<Buffer<T1>> MakeVertexArray(Buffer<T1> buf)
	{
		auto vao_id = genAndBindVao();
		return NewVao(vao_id, std::make_tuple(buf));
	}

	template<typename ... Buffers>
	template<typename ...T1>
	NewVao<Buffers..., Buffer<T1...>> NewVao<Buffers...>::operator+ (Buffer<T1...>&& buf)&&
	{
		constexpr GLsizei stride = sizeof(Buffer<T1...>::value_type) + (0 + ... + sizeof(Buffers::value_type));
		addBufferHelper<Buffer<T1...>>::template addBuffer<stride, 0, 0>();
		return NewVao(this->_vao_id, std::tuple_cat(std::move(this->_buffers), std::make_tuple(std::move(buf)))); //tuple<Buff1, Buff2, ... Buffn
	}

	template<typename ... Buffers>
	template<typename ...T1>
	NewVao<Buffers..., Buffer<T1...>> NewVao<Buffers...>::operator+ (const Buffer<T1...> &buf)&&
	{
		return std::move(*this) + std::move(buf); // TODO: Robi
	}

	template<typename Buffer2, typename ...T1>
	NewVao<Buffer<T1...>, Buffer2> operator+ (Buffer<T1...> buf1, Buffer2 buf2)
	{
		const auto vao_id = genAndBindVao();
		return NewVao(vao_id, std::tuple<>()) + buf1; // + buf2;

		/*constexpr GLsizei stride1 = sizeof(Buffer1::template value_type);
		constexpr GLsizei stride2 = sizeof(Buffer2::template value_type);
		
		addBufferHelper<Buffer1>::template addBuffer<stride1, 0, 0>();
		addBufferHelper<Buffer2>::template addBuffer<stride2, 0, 0>();

		return NewVao(vao_id, std::tuple<Buffer1, Buffer2>(buf1, buf2));*/
	}

	template<typename ... Buffers>
	void NewVao<Buffers...>::Copy()
	{
		auto vao_id = genAndBindVao();
		return std::apply(	//TODO: Robi
				[&](auto&& ...buffs) {
				return NewVao(vao_id, std::tuple<>()) + ... + buffs;
				}, this->_buffers);
	}

	template<typename Type_First, typename ...Types>
	template <GLsizei stride, GLsizei index, GLsizei offset>
	void df::addBufferHelper<df::Buffer<Type_First, Types...>>::addBuffer()
	{
		// sizeof(Type_First) + sizeof...(Types);
		if constexpr (!is_dummy_t_v<Type_First>)
		{
			// Add buffer

			using attib_t = as_array_type_t<Type_First>;
			using base_t = std::remove_pointer_t<std::remove_all_extents_t<attib_t>>;

			glEnableVertexAttribArray(index);
			std::cout << "glEnableAttrib(" << index << ");\n";

			if constexpr (std::is_same_v<base_t, double>)
			{
				const auto components = _GetInternalFormat<uint8_t>::GetComponentCount();
				const auto is_floating = _GetInternalFormat<uint8_t>::IsFloating();

				if constexpr (is_floating)
				{
					glVertexAttribPointer(index, components, df::getOpenGLType<base_t>(), false /*???*/, stride, reinterpret_cast<void*>(offset));
					std::cout << "glVertexAttribPointer(index: " << index << ");\n";
				}
				else
				{
					glVertexAttribIPointer(index, components, df::getOpenGLType<base_t>(), stride, reinterpret_cast<void*>(offset));
					std::cout << "glVertexAttribIPointer(index: " << index << ");\n";
				}
			}

			addBuffer<stride, index + 1, offset + sizeof(Type_First), Types...>();
		}
		else
		{
			addBuffer<stride, index, offset + sizeof(Type_First), Types...>();
		}
	}



	/*template<typename Type_First, typename ...Types>
	struct addBufferHelper<Buffer<Type_First, Types...>>
	{
		template <GLsizei stride, GLsizei index, GLsizei offset>
		void addBuffer()
		{

		}
	};*/
}