workspace "Hawk"
	architecture "x86_64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines 
		{
			"HWK_PLATFORM_WINDOWS",
			"HWK_BUILD_DLL"
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