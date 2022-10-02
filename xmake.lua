set_project("fox_engine")

add_rules("mode.debug", "mode.release")

-- set_languages("c++17")
set_languages("cxx17")

-- add_syslinks("pthread")

-- FOX_PLUGIN_DIRECTORY = "$(shell pwd)"
add_defines("FOX_PLUGIN_DIRECTORY=\"/home/samuel/Git Repos/FoxEngine/\"")

includes("Fox")
includes("plugins")
includes("editor")
includes("sandbox")