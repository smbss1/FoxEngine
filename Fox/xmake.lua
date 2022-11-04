
add_requires("yaml-cpp", "glfw", "opengl")
add_requires("box2d")
add_requires("mono")

includes "vendor"

target "fox_engine"
    set_kind "static"
    set_languages "cxx17"

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

    add_includedirs("vendor/entt/include", { public = true })
    add_includedirs("vendor/ImGuizmo/ImGuizmo", { public = true })
    add_includedirs("vendor/monos/include", { public = true })
    add_files "vendor/ImGuizmo/*.cpp"

    add_includedirs("src/Fox", { public = true })
    add_includedirs("src/Platform", { public = true })
    add_files "src/**.cpp"
