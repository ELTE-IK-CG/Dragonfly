#pragma once

#include "Dragonfly/detail/Buffer/Buffer.h"
#include <iostream>

namespace df
{
	template<typename T_type>struct dummy_t { T_type dummy; using type = T_type; };
	template<typename D> struct is_dummy_t : std::false_type {};
	template<typename T> struct is_dummy_t<dummy_t<T>> : std::true_type {};
	template<typename D> constexpr bool is_dummy_t_v = is_dummy_t< std::decay_t<D>>::value;

	template <typename T> struct as_array_type { using type = T; };
	template <typename T> struct as_array_type<glm::vec<1, T>> { using type = T[1]; };
	template <typename T> struct as_array_type<glm::vec<2, T>> { using type = T[2]; };
	template <typename T> struct as_array_type<glm::vec<3, T>> { using type = T[3]; };
	template <typename T> struct as_array_type<glm::vec<4, T>> { using type = T[4]; };
	template <typename T> using as_array_type_t = typename as_array_type<std::decay_t<T>>::type;

	namespace detail
	{
		template<typename ...TT>
		struct VAO_Index_Calc {
			//constexpr int index = 0; //error if this is needed
		};

		template<>
		struct VAO_Index_Calc<> {
			constexpr int value = 0;
		};

		template<typename T1, typename ...TT>
		struct VAO_Index_Calc<T1, TT...> {
			constexpr int value = 1 + VAO_Index_Calc<TT...>::value;
		};

		template<typename T1, typename ...TT>
		struct VAO_Index_Calc<dummy_t<T1>, TT...> {
			constexpr int value = 0 + VAO_Index_Calc<TT...>::value;
		};

		template<typename T>
		struct VAO_Index_Calc<integral<T>> {
		};

		template <unsigned index_>
		struct VAO_compile_data
		{
			static constexpr unsigned index() { return index_; }

			template<unsigned index, typename ... Buffs> using _addBuffer_t = VAO_compile_data<VAO_Index_Calc<Buffs...>::value>;
		};
	}

	inline GLuint genAndBindVao()
	{
		GLuint id;
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		return id;
	}

	class NewVaoBase
	{
	protected:
		GLuint _vao_id;
		GLuint _curr_attrib_index;

		NewVaoBase(const GLuint id) : _vao_id(id), _curr_attrib_index(0) {}

		void bind() const { glBindVertexArray(_vao_id); }
	};

	template<typename compile_data, typename ... Buffers>
	class NewVao : public NewVaoBase
	{
	public:
		using Compile_Data = compile_data;

		std::tuple<Buffers...> _buffers;

		template<typename ...T>
		using _add_Buffer_t = NewVao<typename compile_data::template _addBuffer_t<T...>, Buffers..., Buffer<T...>>;

		/*template <typename BufferFormat>
		NewVao(df::Buffer<BufferFormat>& buf1)
		{
			std::get<0>(_buffers) = buf1;
		}*/

		NewVao(const GLuint id, std::tuple<Buffers...>&& buffers) : NewVaoBase(id), _buffers(std::move(buffers)) {}

		//template<typename ...T1>
		//NewVao<Buffers..., Buffer<T1...>> operator+ (Buffer<T1...>&& buf)&&;

		//template<typename ...T1>
		//NewVao<Buffers..., Buffer<T1...>> operator+ (const Buffer<T1...>& buf)&&;

		template<typename ...T1>
		_add_Buffer_t<T1...> operator+ (const Buffer<T1...>& buf)&&;

		void Copy();

	};

	template <typename compile_data, typename ...T1>
	NewVao<detail::VAO_compile_data<0>>::_add_Buffer_t<T1...> MakeVertexArray(Buffer<T1...> buf);

	template<typename compile_data, typename Buffer2, typename ...T1>
	NewVao<Buffer<T1...>, Buffer2> operator+ (Buffer<T1...> buf1, Buffer2 buf2);

	template<typename NotABuffer>
	struct addBufferHelper
	{
		//static_assert(false, "Invalid struct");
	};

	template<>
	struct addBufferHelper<Buffer<>>
	{
		template <GLsizei stride, GLsizei index, GLsizei offset>
		static void addBuffer() {}
	};

	template<typename Type_First, typename ...Types>
	struct addBufferHelper<Buffer<Type_First, Types...>>
	{
		template <GLsizei stride, GLsizei index, GLsizei offset>
		static void addBuffer();
	};

}

#include "NewVao.inl"


/*template<typename E>
class V1 {
	using value_type = E;
};

template< typename V>
class A1 {
	V::value_type e;
};*/