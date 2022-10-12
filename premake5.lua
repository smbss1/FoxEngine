workspace " Fox Engine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "FoxCore"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Fox/vendor/glfw"