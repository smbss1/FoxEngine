-- project "entt"
-- 	kind "StaticLib"
-- 	language "C++"
-- 	staticruntime "on"

-- 	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("build/.objs/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"include/entt.hpp"
-- 	}

-- 	filter "configurations:Debug"
-- 		runtime "Debug"
-- 		symbols "on"

-- 	filter "configurations:Release"
-- 		runtime "Release"
-- 		optimize "on"

-- Define how to use the project
usage "entt"
   includedirs { "./include" }