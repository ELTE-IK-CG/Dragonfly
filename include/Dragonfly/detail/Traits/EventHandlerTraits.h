#pragma once

namespace df
{

namespace detail
{
	template<typename> struct sfinae_true : std::true_type {};

#define TRAIT_TEST_STRUCT0(TRAIT_NAME) \
	template<typename T> \
	static auto test_##TRAIT_NAME(int) -> sfinae_true<decltype(std::declval<T>().TRAIT_NAME())>; \
	template<typename T> \
	static auto test_##TRAIT_NAME(long) -> std::false_type;
#define TRAIT_TEST_STRUCT1(TRAIT_NAME) \
	template<typename T, typename A1> \
	static auto test_##TRAIT_NAME(int) -> sfinae_true<decltype(std::declval<T>().TRAIT_NAME(std::declval<A1>()))>; \
	template<typename T, typename A1> \
	static auto test_##TRAIT_NAME(long) -> std::false_type;
#define TRAIT_TEST_STRUCT2(TRAIT_NAME) \
	template<typename T, typename A1, typename A2> \
	static auto test_##TRAIT_NAME(int) -> sfinae_true<decltype(std::declval<T>().TRAIT_NAME(std::declval<A1>(), std::declval<A2>()))>; \
	template<typename T, typename A1, typename A2> \
	static auto test_##TRAIT_NAME(long) -> std::false_type;
#define TRAIT_TEST_STATIC_STRUCT0(TRAIT_NAME) \
	template<typename T> \
	static auto test_static_##TRAIT_NAME(int) -> sfinae_true<decltype(T::TRAIT_NAME())>; \
	template<typename T> \
	static auto test_static_##TRAIT_NAME(long) -> std::false_type;
#define TRAIT_TEST_STATIC_STRUCT1(TRAIT_NAME) \
	template<typename T, typename A1> \
	static auto test_static_##TRAIT_NAME(int) -> sfinae_true<decltype(T::TRAIT_NAME(std::declval<A1>()))>; \
	template<typename T, typename A1> \
	static auto test_static_##TRAIT_NAME(long) -> std::false_type;
#define TRAIT_TEST_STATIC_STRUCT2(TRAIT_NAME) \
	template<typename T, typename A1, typename A2> \
	static auto test_static_##TRAIT_NAME(int) -> sfinae_true<decltype(T::TRAIT_NAME(std::declval<A1>(), std::declval<A2>()))>; \
	template<typename T, typename A1, typename A2> \
	static auto test_static_##TRAIT_NAME(long) -> std::false_type;

	TRAIT_TEST_STRUCT1(HandleKeyUp)
	TRAIT_TEST_STRUCT1(HandleKeyDown)
	TRAIT_TEST_STRUCT1(HandleMouseUp)
	TRAIT_TEST_STRUCT1(HandleMouseDown)
	TRAIT_TEST_STRUCT1(HandleMouseMotion)
	TRAIT_TEST_STRUCT1(HandleMouseWheel)
	TRAIT_TEST_STRUCT2(HandleResize)
	TRAIT_TEST_STATIC_STRUCT1(HandleKeyUp)
	TRAIT_TEST_STATIC_STRUCT1(HandleKeyDown)
	TRAIT_TEST_STATIC_STRUCT1(HandleMouseUp)
	TRAIT_TEST_STATIC_STRUCT1(HandleMouseDown)
	TRAIT_TEST_STATIC_STRUCT1(HandleMouseMotion)
	TRAIT_TEST_STATIC_STRUCT1(HandleMouseWheel)
	TRAIT_TEST_STATIC_STRUCT2(HandleResize)

#undef TRAIT_TEST_STRUCT0
#undef TRAIT_TEST_STRUCT1
#undef TRAIT_TEST_STRUCT2
#undef TRAIT_TEST_STATIC_STRUCT0
#undef TRAIT_TEST_STATIC_STRUCT1
#undef TRAIT_TEST_STATIC_STRUCT2
} // namespace detail

#define HAS_TRAIT0(TRAIT_NAME) \
	template<typename T> \
	struct has_##TRAIT_NAME : decltype(detail::test_##TRAIT_NAME<T>(0)){};
#define HAS_TRAIT1(TRAIT_NAME) \
	template<typename T, typename A1> \
	struct has_##TRAIT_NAME : decltype(detail::test_##TRAIT_NAME<T, A1>(0)){};
#define HAS_TRAIT2(TRAIT_NAME) \
	template<typename T, typename A1, typename A2> \
	struct has_##TRAIT_NAME : decltype(detail::test_##TRAIT_NAME<T, A1, A2>(0)){};
#define HAS_STATIC_TRAIT0(TRAIT_NAME) \
	template<typename T> \
	struct has_static_##TRAIT_NAME : decltype(detail::test_static_##TRAIT_NAME<T>(0)){};
#define HAS_STATIC_TRAIT1(TRAIT_NAME) \
	template<typename T, typename A1> \
	struct has_static_##TRAIT_NAME : decltype(detail::test_static_##TRAIT_NAME<T, A1>(0)){};
#define HAS_STATIC_TRAIT2(TRAIT_NAME) \
	template<typename T, typename A1, typename A2> \
	struct has_static_##TRAIT_NAME : decltype(detail::test_static_##TRAIT_NAME<T, A1, A2>(0)){};

	HAS_TRAIT1(HandleKeyUp)
	HAS_TRAIT1(HandleKeyDown)
	HAS_TRAIT1(HandleMouseUp)
	HAS_TRAIT1(HandleMouseDown)
	HAS_TRAIT1(HandleMouseMotion)
	HAS_TRAIT1(HandleMouseWheel)
	HAS_TRAIT2(HandleResize)
	HAS_STATIC_TRAIT1(HandleKeyUp)
	HAS_STATIC_TRAIT1(HandleKeyDown)
	HAS_STATIC_TRAIT1(HandleMouseUp)
	HAS_STATIC_TRAIT1(HandleMouseDown)
	HAS_STATIC_TRAIT1(HandleMouseMotion)
	HAS_STATIC_TRAIT1(HandleMouseWheel)
	HAS_STATIC_TRAIT2(HandleResize)

#undef HAS_TRAIT0
#undef HAS_TRAIT1
#undef HAS_TRAIT2
#undef HAS_STATIC_TRAIT0
#undef HAS_STATIC_TRAIT1
#undef HAS_STATIC_TRAIT2
} // namespace df
