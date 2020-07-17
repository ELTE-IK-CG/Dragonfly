#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"
#include "Dragonfly/detail/Vao/NewVao.h"
#include "Dragonfly/detail/Buffer/BufferInternalFormats.h"

namespace df
{
	template <typename T1>
	NewVao<Buffer<T1>> MakeVertexArray(Buffer<T1> buf)
	{
		auto vao_id = genAndBindVao();
		return NewVao(vao_id, std::make_tuple(buf));
	}

	template<typename ... Buffers>
	template<typename T1>
	NewVao<Buffers..., T1> NewVao<Buffers...>::operator+ (Buffer<T1> buf)&&
	{
		addBuffer<sizeof(T1) + (0 + ... + sizeof(Buffers::value_type)), 0, 0, T1>();
		return NewVao(this->_vao_id, std::tuple_cat(std::move(this->_buffers), std::make_tuple(buf))); //tuple<Buff1, Buff2, ... Buffn
	}

	template<typename ... Buffers>
	template<typename T1>
	NewVao<Buffers..., T1> NewVao<Buffers...>::operator+ (Buffer<T1>& buf)
	{
		addBuffer<sizeof(T1) + (0 + ... + sizeof(Buffers::value_type)), 0, 0, T1>();
		auto vao_id = genAndBindVao();
		return NewVao(vao_id, std::tuple_cat(this->_buffers, std::make_tuple(buf))); //tuple<Buff1, Buff2, ... Buffn
	}

	template<typename T1, typename T2>
	static NewVao<T1, T2> operator+ (T1 buf1, T2 buf2)
	{
		GLuint vao_id;
		glGenVertexArrays(1, &vao_id);
		glBindVertexArray(vao_id);

		addBuffer<sizeof(T1::value_type), 0, 0, T1>();
		addBuffer<sizeof(T2::value_type), 0, 0, T2>();
		
		return NewVao(vao_id, std::tuple<T1, T2>(buf1, buf2));
	}

	template<typename Buffer_T>
	struct addBufferHelper
	{
		static_assert(false, "Invalid struct");
	};

	template<typename Type_First, typename ...Types>
	struct addBufferHelper<Buffer<Type_First, Types...>>
	{
		template <GLsizei stride, GLsizei index, GLsizei offset>
		void addBuffer()
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
					const auto is_integral = _GetInternalFormat<uint8_t>::IsIntegral();

					if constexpr (is_integral)
					{
						glVertexAttribIPointer(index, components, df::getOpenGLType<base_t>(), stride, reinterpret_cast<void*>(offset));
						std::cout << "glVertexAttribIPointer(index: " << index << ");\n";
					}
					else
					{
						glVertexAttribPointer(index, components, df::getOpenGLType<base_t>(), false /*???*/, stride, reinterpret_cast<void*>(offset));
						std::cout << "glVertexAttribPointer(index: " << index << ");\n";
					}
				}

				addBuffer<stride, index + 1, offset + sizeof(Type_First), Types...>();
			}
			else
			{
				addBuffer<stride, index, offset + sizeof(Type_First), Types...>();
			}
		}
	};
}