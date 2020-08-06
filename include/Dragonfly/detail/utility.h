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


#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_TRACE	 0
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_DEBUG	 1
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_INFO	 2
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_HINT	 3
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_NOTICE	 4
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_WARNING  5
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_ALARM	 6
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_ERROR	 7
#define DF_DETAIL_LOGGER_ENTRY_SEVERITY_FATAL	 8

// Macros should not depend on Debug/Release mode directly

#define _SEV_ df::detail::Logger::Entry::SEVERITY
#define _TYPE_ df::detail::Logger::Entry::TYPE

#define DF_DETAIL_LOCATION df::detail::Logger::Entry::Location{ __FILE__, __LINE__ }

#define DF_DETAIL_CHECK_IMPL(sev, exp, msg) CheckImpl(df::detail::Logger::Entry(sev, DF_DETAIL_LOCATION, (#exp), (msg), _TYPE_::USER), exp)
#define DF_DETAIL_ASSERT_IMPL(sev, exp, msg) if(!(exp)) df::Logger.AddEntry(df::detail::Logger::Entry(sev, DF_DETAIL_LOCATION, (#exp), (msg), _TYPE_::USER));
#define DF_DETAIL_MESSAGE_IMPL(sev, msg) df::Logger.AddEntry(df::detail::Logger::Entry(sev, DF_DETAIL_LOCATION, "", (msg), _TYPE_::USER));	

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_TRACE		// =	_SEV_::TRACE
#define DF_TRACE_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::TRACE)
#define DF_TRACE_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::TRACE)
#define DF_TRACE_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::TRACE)
#else
#define DF_TRACE_CHECK	(expr, msg)		true
#define DF_TRACE_ASSERT	(expr, msg)
#define DF_TRACE_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_DEBUG		// =	_SEV_::DEBUG
#define DF_DEBUG_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::DEBUG)
#define DF_DEBUG_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::DEBUG)
#define DF_DEBUG_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::DEBUG)
#else
#define DF_DEBUG_CHECK	(expr, msg)		true
#define DF_DEBUG_ASSERT	(expr, msg)
#define DF_DEBUG_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_INFO		// =	_SEV_::INFO
#define DF_INFO_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::INFO)
#define DF_INFO_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::INFO)
#define DF_INFO_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::INFO)
#else
#define DF_INFO_CHECK	(expr, msg)		true
#define DF_INFO_ASSERT	(expr, msg)
#define DF_INFO_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_HINT		// =	_SEV_::HINT
#define DF_HINT_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::HINT)
#define DF_HINT_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::HINT)
#define DF_HINT_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::HINT)
#else
#define DF_HINT_CHECK	(expr, msg)		true
#define DF_HINT_ASSERT	(expr, msg)
#define DF_HINT_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_NOTICE		// =	_SEV_::NOTICE
#define DF_NOTICE_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::NOTICE)
#define DF_NOTICE_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::NOTICE)
#define DF_NOTICE_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::NOTICE)
#else
#define DF_NOTICE_CHECK	(expr, msg)		true
#define DF_NOTICE_ASSERT	(expr, msg)
#define DF_NOTICE_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_WARNING		// =	_SEV_::WARNING
#define DF_WARNING_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::WARNING)
#define DF_WARNING_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::WARNING)
#define DF_WARNING_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::WARNING)
#else
#define DF_WARNING_CHECK	(expr, msg)		true
#define DF_WARNING_ASSERT	(expr, msg)
#define DF_WARNING_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_ALARM		// =	_SEV_::ALARM
#define DF_ALARM_CHECK	(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::ALARM)
#define DF_ALARM_ASSERT	(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::ALARM)
#define DF_ALARM_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::ALARM)
#else
#define DF_ALARM_CHECK	(expr, msg)		true
#define DF_ALARM_ASSERT	(expr, msg)
#define DF_ALARM_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_ERROR			// =	_SEV_::ERROR
#define DF_ERROR_CHECK		(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::ERROR)
#define DF_ERROR_ASSERT		(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::ERROR)
#define DF_ERROR_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::ERROR)
#else
#define DF_ERROR_CHECK		(expr, msg)		true
#define DF_ERROR_ASSERT		(expr, msg)
#define DF_ERROR_MESSAGE	(msg)
#endif

#if DF_LOG_SEVERITY >= DF_DETAIL_LOGGER_ENTRY_SEVERITY_FATAL			// =	_SEV_::FATAL
#define DF_FATAL_CHECK		(expr, msg)		DF_DETAIL_CHECK_IMPL	(expr, msg, _SEV_::FATAL)
#define DF_FATAL_ASSERT		(expr, msg)		DF_DETAIL_ASSERT_IMPL	(expr, msg, _SEV_::FATAL)
#define DF_FATAL_MESSAGE	(msg)			DF_DETAIL_MESSAGE_IMPL	(msg,		_SEV_::FATAL)
#else
#define DF_FATAL_CHECK		(expr, msg)		true
#define DF_FATAL_ASSERT		(expr, msg)
#define DF_FATAL_MESSAGE	(msg)
#endif

#undef _SEV_