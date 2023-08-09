#pragma once

#include <memory>

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>


namespace Hawk {
	class  Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

}


//Core
#define HWK_CORE_TRACE(...)		::Hawk::Log::GetCoreLogger()->trace(__VA_ARGS__) //Macro for Logger
#define HWK_CORE_INFO(...)		::Hawk::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HWK_CORE_WARN(...)		::Hawk::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HWK_CORE_ERROR(...)		::Hawk::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HWK_CORE_CRITICAL(...)	::Hawk::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client
#define HWK_TRACE(...)		::Hawk::Log::GetClientLogger()->trace(__VA_ARGS__) //Macro for Logger
#define HWK_INFO(...)		::Hawk::Log::GetClientLogger()->info(__VA_ARGS__)
#define HWK_WARN(...)		::Hawk::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HWK_ERROR(...)		::Hawk::Log::GetClientLogger()->error(__VA_ARGS__)
#define HWK_CRITICAL(...)	::Hawk::Log::GetClientLogger()->critical(__VA_ARGS__)