set_project("fox_engine")

add_rules("mode.debug", "mode.release")

set_languages("cxx17")

if rule("mode.debug") then
    add_defines("FOX_DEBUG")
end
if rule("mode.release") then
    add_defines("FOX_RELEASE")
end

includes("Fox")
includes("plugins")
includes("FoxEditor")
includes("sandbox")