set_project("fox_engine")

add_rules("mode.debug", "mode.release")

set_languages("cxx17")

if rule("mode.debug") then
    add_defines("FOX_DEBUG")
end
if rule("mode.release") then
    add_defines("FOX_RELEASE")
end

package("mono")

    set_homepage("https://www.mono-project.com/")
    set_description("Cross platform, open source .NET development framework")

    set_urls("https://download.mono-project.com/sources/mono/mono-$(version).tar.xz",
             {version = function (version) return version:gsub("%+", ".") end})

    add_versions("6.10.0+105", "df7816daf4fbba9b98b997ae24bc3f8aa1e38386")

    add_includedirs("include/mono-2.0")

    on_install(function (package)
        local configs = {"--disable-silent-rules", "--enable-nls=no"}
        import("package.tools.autoconf").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("mono_object_get_class", {includes = "mono/metadata/object.h"}))
    end)

includes("Fox")
includes("plugins")
includes("FoxEditor")
includes("sandbox")