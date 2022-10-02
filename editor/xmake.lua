
target("fox_editor")
    set_kind("binary")
    set_targetdir("./")

    add_deps("fox_engine")

    add_includedirs("include")
    add_files("src/*.cpp")
