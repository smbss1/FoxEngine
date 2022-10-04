
-- add_requires("rttr")
add_requires("yaml-cpp", "glfw", "opengl")
add_requires("box2d")
add_requires("mono")

includes "vendor"

target "fox_engine"
    set_kind "static"
    set_languages "cxx17"

    -- add_defines("FOX_PLUGIN_DIRECTORY=$(FOX_PLUGIN_DIRECTORY)", { public = true })
    -- add_defines "FOX_BUILD_LIB=1"
    -- add_defines "_REENTRANT"

    -- add_cxflags("-I/usr/lib/pkgconfig/../../include/mono-2.0 -D_REENTRANT -L/usr/lib/pkgconfig/../../lib -lmono-2.0 -lm -lpthread", {force=true})
    -- add_includedirs("/usr/lib/pkgconfig/../../include/mono-2.0", { public = true })

    -- add_linkdirs("/usr/lib/pkgconfig/../../lib")
    -- add_links("mono-2.0")
    -- add_links("mono-native")
    -- add_links("m")

    -- add_packages "rttr"
    add_packages "yaml-cpp"
    add_packages "glfw"
    add_packages "opengl"
    add_packages "box2d"
    add_packages "mono"

    add_deps "imgui"
    add_deps "fox_json"
    add_deps "glm"
    add_deps "glad"

    add_syslinks "dl"
    add_syslinks "pthread"

    add_includedirs("src/Fox", { public = true })
    add_includedirs("src/Platform", { public = true })
    add_files "src/**.cpp"

    add_includedirs("vendor/entt/include", { public = true })
    add_includedirs("vendor/ImGuizmo/ImGuizmo", { public = true })
    add_files "vendor/ImGuizmo/*.cpp"

-- target_compile_definitions(fox_engine PUBLIC
--     $<$<CONFIG:DEBUG>:FOX_DEBUG=1>
-- )
