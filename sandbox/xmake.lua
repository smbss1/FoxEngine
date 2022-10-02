
target("sandbox")
    set_kind("binary")
    set_targetdir("./")

    add_deps("fox_engine")
    -- add_deps("fox_opengl")

    add_includedirs("include")
    add_files("src/*.cpp")
