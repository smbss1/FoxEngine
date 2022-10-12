project "mono"
	kind "StaticLib"
	language "C++"

	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("build/.objs/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/**.h",
	}

	-- includedirs { "include" }

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"