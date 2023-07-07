project "Vulkan"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/vendor/Vulkan/Include/**.h",
		"%{prj.name}/vendor/Vulkan/Include/**.cpp"
	}

	includedirs
	{
		"Include"
	}


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		buildoptions "/MT"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"