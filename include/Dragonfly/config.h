#pragma once

#include "Dragonfly/detail/Events/LogManager.h"

#define OPENGL_VERSION 45

//Todo: .exe shipped with framework that is run on (re)build and determines the version

//Define if you want colorful output
#define USE_COLOR_CODES


#if defined(_DEBUG) || defined(GPU_DEBUG)


//TODO REMOVE:
//namespace for elte cg hidden stuff
namespace eltecg {
	namespace detail {
		constexpr unsigned int max_warn_count = 64;
		static unsigned int warn_count = 0;
		enum class MESSAGE_TYPE { WARNING = 0, ASSERT = 1, GPU_WARNING = 2, GPU_ASSERT = 3, NUM_MESSAGE_TYPES };
		//void print_msg(MESSAGE_TYPE type, const char* file, int line, const char* msg, const char* expr);
		void gpu_check(const char* file, int line);
	}
}
#endif

/* Defines*/
//TODO REMOVE:
#ifdef _DEBUG
namespace df { constexpr bool IS_THIS_DEBUG = true; }
//#define WARNING(expr, msg) if((expr) && ++eltecg::detail::warn_count <= eltecg::detail::max_warn_count) \
//		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::WARNING ,__FILE__, __LINE__, (msg), #expr )
//#define ASSERT(expr, msg) if(!(expr)) \
//		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::ASSERT ,__FILE__, __LINE__, (msg), #expr )
#define INFO(expr, msg) if(expr) df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::INFO, \
	df::LogManager::TYPE::MESSAGE, { __FILE__, __LINE__ }, (#expr), (msg)), 0, 0);
#define WARNING(expr, msg) if(expr) df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::WARNING, \
	df::LogManager::TYPE::MESSAGE, { __FILE__, __LINE__ }, (#expr), (msg)), 0, 0);
#define ASSERT(expr, msg) if(expr) df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::WARNING, \
	df::LogManager::TYPE::ASSERT, { __FILE__, __LINE__ }, (#expr), (msg)), 0, 0);
#else //_RELEASE
namespace df { constexpr bool IS_THIS_DEBUG = false; }
#define WARNING(expr, msg)
#define ASSERT(expr, msg)
#endif


// GPU_DEBUG mode provides additional checks to be done all the time.
// For example, framebuffer completeness. Usually involves calling additional OpenGL functions
#ifdef GPU_DEBUG
/*#define GPU_WARNING(expr, msg) if((expr) && ++eltecg::detail::warn_count <= eltecg::detail::max_warn_count) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::GPU_WARNING ,__FILE__, __LINE__, (msg), #expr )
#define GPU_ASSERT(expr, msg) if(!(expr)) \
		eltecg::detail::print_msg( eltecg::detail::MESSAGE_TYPE::GPU_ASSERT ,__FILE__, __LINE__, (msg), #expr )
#define GL_CHECK if( eltecg::detail::warn_count <= eltecg::detail::max_warn_count && \
		!eltecg::detail::gpu_check(__FILE__,__LINE__))	++eltecg::detail::warn_count*/

#define GPU_WARNING(expr, msg) df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::WARNING, \
	df::LogManager::TYPE::MESSAGE, { __FILE__, __LINE__ }, (#expr), (msg)), 0, 0);
#define GPU_ASSERT(expr, msg) df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::WARNING, \
	df::LogManager::TYPE::ASSERT, { __FILE__, __LINE__ }, (#expr), (msg)), 0, 0);
#define GL_CHECK eltecg::detail::gpu_check(__FILE__, __LINE__);
//#define GL_CHECK df::Logger.AddEntry(df::LogManager::LogEntry(df::LogManager::SEVERITY::WARNING, \
//	df::LogManager::TYPE::CHECK, { __FILE__, __LINE__ }, ("GL_CHECK"), ("???")), 0, 0);

#else
#define GPU_WARNING(expr, msg)
#define GPU_ASSERT(expr, msg)
#define GL_CHECK
#endif

//NEW LOG
#if _DEBUG

#define DF_LOG_SEVERITY 0
#define DF_LOG_TRACE true
#define DF_LOG_DEBUG true

#else

#define DF_LOG_SEVERITY 7
#define DF_LOG_TRACE false
#define DF_LOG_DEBUG false

#endif
