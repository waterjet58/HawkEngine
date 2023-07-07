workspace "Hawk"
	architecture "x86_64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludedDirectories = {}

IncludedDirectories["GLFW"] = "Hawk/vendor/GLFW/include"
IncludedDirectories["Vulkan"] = "Hawk/vendor/Vulkan/Include"
IncludedDirectories["glm"] = "Hawk/vendor/glm/"

include "Hawk/vendor/GLFW"
include "Hawk/vendor/Vulkan"
include "Hawk/vendor/glm"

project "Hawk"
	location "Hawk"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hwkPrecompiledHeader.h"
	pchsource "Hawk/src/hwkPrecompiledHeader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludedDirectories.GLFW}",
		"%{IncludedDirectories.Vulkan}",
		"%{IncludedDirectories.glm}"
	}

	links
	{
		"GLFW",
		"dwmapi.lib",
		"Hawk/vendor/Vulkan/Lib/vulkan-1.lib"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines 
		{
			"HWK_PLATFORM_WINDOWS",
			"HWK_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HWK_DEBUG"
		symbols "on"
	filter "configurations:Release"
		defines "HWK_RELEASE"
		optimize "on"
	filter "configurations:Dist"
		defines "HWK_DIST"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Hawk/vendor/spdlog/include",
		"Hawk/src"
	}

	links
	{
		"Hawk"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines 
		{
			"HWK_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HWK_DEBUG"
		symbols "on"
	filter "configurations:Release"
		defines "HWK_RELEASE"
		optimize "on"
	filter "configurations:Dist"
		defines "HWK_DIST"
		optimize "on"