#pragma once
#include "config.h"

#ifdef _DEBUG
	#include "debug.h"
	#define WARNING(expr, msg) if((expr) && ++eltecg::detail::warn_count <= eltecg::detail::max_warn_count) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::WARNING ,__FILE__, __LINE__, (msg), #expr )
	#define ASSERT(expr, msg) if(!(expr)) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::ASSERT ,__FILE__, __LINE__, (msg), #expr )
#else //_RELEASE
	#define WARNING(expr, msg)
	#define ASSERT(expr, msg)
#endif

// GPU_DEBUG mode provides additional checks to be done all the time.
// For example, framebuffer completeness. Usually involves calling additional OpenGL functions
#ifdef GPU_DEBUG
	#include "debug.h"
	#define GPU_WARNING(expr, msg) if((expr) && ++eltecg::detail::warn_count <= eltecg::detail::max_warn_count) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::GPU_WARNING ,__FILE__, __LINE__, (msg), #expr )
	#define GPU_ASSERT(expr, msg) if(!(expr)) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::GPU_ASSERT ,__FILE__, __LINE__, (msg), #expr )
	#define GL_CHECK if( eltecg::detail::warn_count <= eltecg::detail::max_warn_count && \
		!eltecg::detail::gpu_check(__FILE__,__LINE__))	++eltecg::detail::warn_count
#else
	#define GPU_WARNING(expr, msg)
	#define GPU_ASSERT(expr, msg)
	#define GL_CHECK
#endif

//This trick is from https://github.com/NVIDIAGameWorks/Falcor/blob/master/Framework/Source/Framework.h
#define ENUM_CLASS_FLAG_OPERATORS(e_)																	\
	inline e_ operator& (e_ a, e_ b){return static_cast<e_>(static_cast<int>(a)& static_cast<int>(b));}	\
    inline e_ operator| (e_ a, e_ b){return static_cast<e_>(static_cast<int>(a)| static_cast<int>(b));}	\
    inline e_& operator|= (e_& a, e_ b){a = a | b; return a;};											\
    inline e_& operator&= (e_& a, e_ b) { a = a & b; return a; };										\
    inline e_  operator~ (e_ a) { return static_cast<e_>(~static_cast<int>(a));}						\
    inline bool is_set(e_ val, e_ flag) { return (val & flag) != (e_)0;}