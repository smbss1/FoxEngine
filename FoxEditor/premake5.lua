project "FoxEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/build/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/.objs/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Fox/src/Fox",
		"%{wks.location}/Fox/src/Platform",
		"%{wks.location}/Fox/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Fox"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "FOX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "FOX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "FOX_DIST"
		runtime "Release"
		optimize "on"