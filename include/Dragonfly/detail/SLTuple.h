#pragma once

#include <cstddef>
#include <type_traits>

template <class... Ts>
#pragma pack()
struct SLTuple;

template <class T, class... Ts>
#pragma pack()
struct SLTuple<T, Ts...>
{
	T first;
	SLTuple<Ts...> rest;
};

template <class T>
#pragma pack()
struct SLTuple<T>
{
	T first;
};

namespace detail {
	template < size_t i, class T>
	struct SLTupleElement;

	template < size_t i, class T, class... Ts>
	struct SLTupleElement<i, SLTuple<T, Ts...> > : SLTupleElement<i - 1, SLTuple<Ts...> > {};

	template <class T, class... Ts>
	struct SLTupleElement<0, SLTuple<T, Ts...> >
	{
		using type = T;
	};

	template <size_t i>
	struct SLTupleAccessor
	{
		template <class... Ts>
		static inline typename SLTupleElement<i, SLTuple<Ts...> >::type& get(SLTuple<Ts...>& t)
		{
			return SLTupleAccessor<i - 1>::get(t.rest);
		}

		template <class... Ts>
		static inline const typename SLTupleElement<i, SLTuple<Ts...> >::type& get(const SLTuple<Ts...>& t)
		{
			return SLTupleAccessor<i - 1>::get(t.rest);
		}
	};

	template <>
	struct SLTupleAccessor<0>
	{
		template <class... Ts>
		static inline typename SLTupleElement<0, SLTuple<Ts...> >::type& get(SLTuple<Ts...>& t)
		{
			return t.first;
		}

		template <class... Ts>
		static inline const typename SLTupleElement<0, SLTuple<Ts...> >::type& get(const SLTuple<Ts...>& t)
		{
			return t.first;
		}
	};

	template <class T, class... Ts>
	struct SLTupleBuilder
	{
		static inline void make(SLTuple<typename std::decay<T>::type, typename ::std::decay<Ts>::type...>& t, T&& x, Ts&&... xs)
		{
			t.first = x;
			SLTupleBuilder<Ts...>::make(t.rest, std::forward<Ts>(xs)...);
		}
	};

	template <class T>
	struct SLTupleBuilder<T>
	{
		static inline void make(SLTuple<typename ::std::decay<T>::type>& t, T&& x)
		{
			t.first = x;
		}
	};

	template <class... Ts>
	inline SLTuple<typename ::std::decay<Ts>::type...> make_tuple(Ts&&... x)
	{
		SLTuple<typename ::std::decay<Ts>::type...> t;
		SLTupleBuilder<Ts...>::make(t, ::std::forward<Ts>(x)...);
		return t;
	}

	template <size_t i, class... Ts>
	inline typename SLTupleElement<i, SLTuple<Ts...> >::type& get(SLTuple<Ts...>& t)
	{
		return SLTupleAccessor<i>::get(t);
	}

	template <size_t i, class... Ts>
	inline const typename SLTupleElement<i, SLTuple<Ts...> >::type& get(const SLTuple<Ts...>& t)
	{
		return SLTupleAccessor<i>::get(t);
	}

}