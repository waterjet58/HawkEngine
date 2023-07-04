workspace "Hawk"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Hawk"
	location "Hawk"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines 
		{
			"HWK_PLATFORM_WINDOWS",
			"HWK_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "HWK_DEBUG"
		symbols "ON"
	filter "configurations:Release"
		defines "HWK_RELEASE"
		optimize "ON"
	filter "configurations:Dist"
		defines "HWK_DIST"
		optimize "ON"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines 
		{
			"HWK_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HWK_DEBUG"
		symbols "ON"
	filter "configurations:Release"
		defines "HWK_RELEASE"
		optimize "ON"
	filter "configurations:Dist"
		defines "HWK_DIST"
		optimize "ON"