
include "Deps.lua"
include "usage.lua"

workspace " Fox Engine"
    architecture "x86_64"
    configurations { "Debug", "Release", "Distribution" }
	startproject "FoxEditor"

    flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Fox/vendor"
group ""

group "Core"
	include "Fox"
	-- include "Fox-ScriptCore"
group ""

group "Tools"
	include "FoxEditor"
group ""

-- group "Misc"
-- 	include "Sandbox"
-- group ""