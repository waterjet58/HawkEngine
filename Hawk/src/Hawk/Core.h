#pragma once

#ifdef HWK_PLATFORM_WINDOWS
	#ifdef HWK_BUILD_DLL
		#define HAWK_API __declspec(dllexport)
	#else
		#define HAWK_API __declspec(dllimport)
	#endif
#else
	#error Hawk only support Windows
#endif