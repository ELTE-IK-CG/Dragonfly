#pragma once
#include "..\config.h"
#include <type_traits>

#define ENUM_CLASS_FLAG_OPERATORS(T)  /* from https://stackoverflow.com/a/58068168 */                                                                                           \
    inline T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }                                                                       \
    inline T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }	\
	inline bool operator && (T a, T b) { return static_cast<bool>(a & b); }

#define ENUM_CLASS_RELATIONAL_OPERATORS(T)                                                                                                                                      \
    inline T operator< (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) < static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator> (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) > static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator<= (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) <= static_cast<std::underlying_type<T>::type>(b) ); }                 \
    inline T operator>= (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) >= static_cast<std::underlying_type<T>::type>(b) ); }


//... 0-4
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_WARNING 5
//... 6
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_ERROR 7
//... 8

// Macros should not depend on Debug/Release mode directly

#define DF_DETAIL_CHECK_IMPL()		//todo
#define DF_DETAIL_ASSERT_IMPL()		//todo
#define DF_DETAIL_MESSAGE_IMPL()	//todo

#define _SEV_ df::detail::Logger::Entry::SEVERITY

//... 0-4

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_WARNING		// =	_SEV_::WARNING
#define DF_WARNING_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::WARNING)
#define DF_WARNING_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::WARNING)
#define DF_WARNING_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::WARNING)
#else
#define DF_WARNING_CHECK	(expr, msg)		true
#define DF_WARNING_ASSERT	(expr, msg)
#define DF_WARNING_MESSAGE	(msg)
#endif

//... 6

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_ERROR			// =	_SEV_::ERROR
#define DF_ERROR_CHECK		(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::ERROR)
#define DF_ERROR_ASSERT		(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::ERROR)
#define DF_ERROR_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::ERROR)
#else
#define DF_ERROR_CHECK		(expr, msg)		true
#define DF_ERROR_ASSERT		(expr, msg)
#define DF_ERROR_MESSAGE	(msg)
#endif

//... 8

#undef _SEV_