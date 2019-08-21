#pragma once //TODO: maybe ifdef header guards
//Configurations for the elte cg libary

#ifndef OPENGL_VERSION
//OpenGL version parameter (without dot) (should be in the 33-35 range)
#define OPENGL_VERSION 45
//why not contexpr? Becuase no contexpr if in C++11
#endif

#ifndef USE_COLOR_CODES
//Define if you want colorful output
#define USE_COLOR_CODES

#endif

//Tip: create a build configuration for this
//#define GPU_DEBUG

#ifndef ENABLE_GLOBAL_BIND_OPTIMIZATION
//Enables global binding optimization
#define ENABLE_GLOBAL_BIND_OPTIMIZATION
#endif

#ifndef DISABLE_GLOBAL_BIND_OPTIMIZATION
//Disables global binding optimization
#define DISABLE_GLOBAL_BIND_OPTIMIZATION
#endif

//TODO: for each class ENABLE and DISABLE locally overwriting global parameter