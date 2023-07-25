workspace "Hawk"
	architecture "x86_64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
VULKAN_SDK = os.getenv("VULKAN_SDK")


IncludedDirectories = {}

IncludedDirectories["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludedDirectories["GLFW"] = "Hawk/vendor/GLFW/include"
IncludedDirectories["imgui"] = "Hawk/vendor/imgui"
IncludedDirectories["glm"] = "VulkanProject/vendor/glm"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

include "Hawk/vendor/GLFW"
include "Hawk/vendor/imgui"

project "Hawk"
	location "Hawk"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hwkPrecompiledHeader.h"
	pchsource "Hawk/src/hwkPrecompiledHeader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/Shaders",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludedDirectories.GLFW}",
		"%{IncludedDirectories.imgui}",
		"%{IncludedDirectories.VulkanSDK}"
	}

	libdirs
	{
		"%{LibraryDir.VulkanSDK}"
	}

	links
	{
		"GLFW",
		"imgui",
		"vulkan-1.lib"
	}

	defines 
	{
		"GLFW_INCLUDE_NONE"
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


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
		"Hawk/src",
		"Hawk/vendor",
		"%{IncludedDirectories.GLFW}",
		"%{IncludedDirectories.imgui}",
		"%{IncludedDirectories.VulkanSDK}"
	}

	libdirs
	{
		"%{LibraryDir.VulkanSDK}"
	}

	links
	{
		"Hawk",
		"vulkan-1.lib"
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