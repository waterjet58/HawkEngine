project "Vulkan"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/vendor/Vulkan/Include/**.h",
		"%{prj.name}/vendor/Vulkan/Include/**.cpp",
		"%{prj.name}/vendor/Vulkan/Lib/vulkan-1.lib"
	}

	includedirs
	{
		"Include"
	}


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"