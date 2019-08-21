#pragma once
#include "config.h"

#if defined(_DEBUG) || defined(GPU_DEBUG)

//namespace for elte cg hidden stuff
namespace eltecg {	namespace detail {

constexpr unsigned int max_warn_count = 64;
static unsigned int warn_count = 0;
enum class MESSAGE_TYPE {WARNING=0, ASSERT=1, GPU_WARNING=2, GPU_ASSERT=3, NUM_MESSAGE_TYPES = 4};
void print_msg(MESSAGE_TYPE type, const char* file, int line, const char* msg, const char* expr);
bool gpu_check(const char* file, int line);

} } //namespace eltecg::detail

#endif //defined(_DEBUG) || defined(GPU_DEBUG)