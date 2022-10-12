project "glad"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("build/.objs/" .. outputdir .. "/%{prj.name}")

	files
	{
		"glad/glad.h",
		"glad/glad.cpp"
	}

	includedirs "."

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
