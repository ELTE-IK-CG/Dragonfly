#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"

namespace df
{
	template<typename T_type>struct dummy_t { T_type dummy; using type = T_type; };
	template<typename D> struct is_dummy_t : std::false_type {};
	template<typename T> struct is_dummy_t<dummy_t<T>> : std::true_type {};
	template<typename D> constexpr bool is_dummy_t_v = is_dummy_t< std::decay_t<D>>::value;

	template <GLsizei stride, GLsizei index, GLsizei offset, typename Type_First, typename ...Types>
	void addBuffer()
	{
		glEnableVertexAttribArray(index);
		GL_CHECK;

		// sizeof(Type_First) + sizeof...(Types);
		if constexpr (!is_dummy_t_v<Type_First>)
		{
			// Add buffer
			addBuffer<stride, index + 1, offset + sizeof(Type_First), Types...>();
		}
		else
		{
			addBuffer<stride, index, offset + sizeof(Type_First), Types...>();
		}
	}

	inline GLuint genAndBindVao()
	{
		GLuint id;
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		return id;
	}

	template<typename ... Buffers>
	class NewVao
	{
	protected:
		GLuint _vao_id;
		GLuint _curr_attrib_index;

	public:
		std::tuple<Buffers...> _buffers;

		template <typename T1>
		NewVao(df::Buffer<T1>& buf1)
		{
			std::get<0>(_buffers) = buf1;
		}

		NewVao(const GLuint id, std::tuple<Buffers...>&& buffers) : _vao_id(id), _buffers(std::move(buffers)) {}

		void bind() const { glBindVertexArray(_vao_id); }

		template<typename T1>
		NewVao<Buffers..., T1> operator+ (df::Buffer<T1> buf)&&
		{
			addBuffer<sizeof(T1) + (0 + ... + sizeof(Buffers::value_type)), 0, 0, T1>();

			return NewVao(this->_vao_id, std::tuple_cat(std::move(this->_buffers), std::make_tuple(buf))); //tuple<Buff1, Buff2, ... Buffn
		}

		template<typename T1>
		NewVao<Buffers..., T1> operator+ (df::Buffer<T1>& buf)
		{
			addBuffer<sizeof(T1) + (0 + ... + sizeof(Buffers::value_type)), 0, 0, T1>();

			auto vao_id = genAndBindVao();

			return NewVao(vao_id, std::tuple_cat(this->_buffers, std::make_tuple(buf))); //tuple<Buff1, Buff2, ... Buffn
		}

	};

	template <typename T1>
	df::NewVao<df::Buffer<T1>> MakeVertexArray(df::Buffer<T1> buf)
	{
		auto vao_id = genAndBindVao();
		return NewVao(vao_id, std::make_tuple(buf));
	}

	/*template<typename T1, typename ...Buffers>
	static NewVao<Buffers..., T1> operator+ (NewVao<Buffers...>&& vao, df::Buffer<T1> buf)
	{
		addBuffer<sizeof(T1) + (0 + ... + sizeof(Buffers::value_type)), 0, 0, T1>();

		return NewVao(vao.vao_id, std::tuple_cat(std::move(vao._buffers), std::make_tuple(buf))); //tuple<Buff1, Buff2, ... Buffn
	}*/

	template<typename T1, typename T2>
	static NewVao<T1, T2> operator+ (T1 buf1, T2 buf2)
	{
		GLuint vao_id;
		glGenVertexArrays(1, &vao_id);
		glBindVertexArray(vao_id);

		addBuffer<sizeof(T1::value_type) + sizeof(T1::value_type), 0, 0, T1, T2>();

		return NewVao(vao_id, std::tuple<T1, T2>(buf1, buf2));
	}



}


/*template<typename E>
class V1 {
	using value_type = E;
};

template< typename V>
class A1 {
	V::value_type e;
};*/