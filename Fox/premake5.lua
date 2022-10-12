project "Fox"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/build/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/.objs/" .. outputdir .. "/%{prj.name}")

	-- pchheader "foxpch.hpp"
	-- pchsource "src/foxpch.cpp"

	files
	{
		"src/**.hpp",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src/Fox",
		"src/Platform",

		"%{IncludeDir.box2d}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
	}

	links
	{
		"box2d",
		"glfw",
		"glad",
		"ImGui",
		"yaml-cpp",
		"OpenGL32",

		"%{Library.mono}",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
		}

	filter "configurations:Debug"
		defines "FOX_DEBUG"
		runtime "Debug"
		symbols "on"

		-- links
		-- {
		-- 	"%{Library.ShaderC_Debug}",
		-- 	"%{Library.SPIRV_Cross_Debug}",
		-- 	"%{Library.SPIRV_Cross_GLSL_Debug}"
		-- }

	filter "configurations:Release"
		defines "FOX_RELEASE"
		runtime "Release"
		optimize "on"

		-- links
		-- {
		-- 	"%{Library.ShaderC_Release}",
		-- 	"%{Library.SPIRV_Cross_Release}",
		-- 	"%{Library.SPIRV_Cross_GLSL_Release}"
		-- }

	filter "configurations:Distribution"
		defines "FOX_DIST"
		runtime "Release"
		optimize "on"

		-- links
		-- {
		-- 	"%{Library.ShaderC_Release}",
		-- 	"%{Library.SPIRV_Cross_Release}",
		-- 	"%{Library.SPIRV_Cross_GLSL_Release}"
		-- }