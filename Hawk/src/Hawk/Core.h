#pragma once

#ifdef HWK_PLATFORM_WINDOWS
	
#else
	#error Hawk only support Windows
#endif

#ifdef HWK_ENABLE_ASSERTS
	#define HWK_ASSERT(x, ...) { if(!(x)) HWK_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define HWK_CORE_ASSERT(x, ...) { if(!(x)) { HWK_CORE_ASSERT("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define HWK_ASSERT(x, ...)
	#define HWK_CORE_ASSERT(x, ...)
#endif

#define Bit(x) (1 << x)

#include "Hawk/Log.h"


