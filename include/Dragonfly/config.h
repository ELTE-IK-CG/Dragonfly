#pragma once

#define OPENGL_VERSION 45

//Todo: .exe shipped with framework that is run on (re)build and determines the version

//Define if you want colorful output
#define USE_COLOR_CODES

//#define GPU_DEBUG //You should have a build configuration for this


/* Debug stuff*/
// TODO: Make it better

#if defined(_DEBUG) || defined(GPU_DEBUG)

//namespace for elte cg hidden stuff
namespace eltecg {
	namespace detail {
		constexpr unsigned int max_warn_count = 64;
		static unsigned int warn_count = 0;
		enum class MESSAGE_TYPE { WARNING = 0, ASSERT = 1, GPU_WARNING = 2, GPU_ASSERT = 3, NUM_MESSAGE_TYPES };
		void print_msg(MESSAGE_TYPE type, const char* file, int line, const char* msg, const char* expr);
		bool gpu_check(const char* file, int line);
	}
}
#endif

/* Defines*/

#ifdef _DEBUG
namespace df { constexpr bool IS_THIS_DEBUG = true; }
#define WARNING(expr, msg) if((expr) && ++eltecg::detail::warn_count <= eltecg::detail::max_warn_count) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::WARNING ,__FILE__, __LINE__, (msg), #expr )
#define ASSERT(expr, msg) if(!(expr)) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::ASSERT ,__FILE__, __LINE__, (msg), #expr )
#else //_RELEASE
namespace df { constexpr bool IS_THIS_DEBUG = false; }
#define WARNING(expr, msg)
#define ASSERT(expr, msg)
#endif

// GPU_DEBUG mode provides additional checks to be done all the time.
// For example, framebuffer completeness. Usually involves calling additional OpenGL functions
#ifdef GPU_DEBUG
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

#include <type_traits> // The following is from https://stackoverflow.com/a/58068168
#define ENUM_CLASS_FLAG_OPERATORS(T)                                                                                                                                            \
    inline T operator~ (T a) { return static_cast<T>( ~static_cast<std::underlying_type<T>::type>(a) ); }                                                                       \
    inline T operator| (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator& (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T operator^ (T a, T b) { return static_cast<T>( static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b) ); }                   \
    inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b) ); }   \
    inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b) ); }	\
	inline bool operator && (T a, T b) { return static_cast<bool>(a & b); }

