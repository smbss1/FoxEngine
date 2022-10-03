

package("yaml-cpp")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "yaml-cpp"))

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

-- package("imgui")
--     add_deps("cmake")
--     set_sourcedir(path.join(os.scriptdir(), "imgui"))

--     on_install(function (package)
--         local configs = {}
--         table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
--         table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
--         import("package.tools.cmake").install(package, configs)
--     end)
-- package_end()

-- package("imgui-gizmo")
--     add_deps("cmake")
--     set_sourcedir(path.join(os.scriptdir(), "ImGuizmo"))

--     on_install(function (package)
--         local configs = {}
--         table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
--         table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
--         -- add_packages("imgui")
--         import("package.tools.cmake").install(package, configs)
--     end)
-- package_end()

target("glm")
    set_kind("static")
    add_includedirs("glm", { public = true })
    add_files("glm/**.cpp")

target("glad")
    set_kind("static")
    add_includedirs("glad", { public = true })
    add_files("glad/**.cpp")

includes("imgui", "FoxJson")
