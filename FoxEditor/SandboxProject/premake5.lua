-- FoxRootDirectory = os.getenv("FOX_DIR")

-- NOTE: Finding a way to set in env variable in linux or in getting the value in a file config
FoxRootDirectory = "C:\\Users\\besse\\Documents\\GitHub\\FoxEngine\\"
-- include (path.join(FoxRootDirectory, "FoxEditor", "Resources", "LUA", "Fox.lua"))

local function getAssemblyFiles(directory, is_windows)
	if is_windows then
		handle = io.popen("dir " .. directory .. " /B /A-D")
	end

	if is_windows == false then
		handle = io.popen("ls \"".. directory .. "\"/*.dll")
		print(handle)
	end

	t = {}
	for f in handle:lines() do
		if path.hasextension(f, ".dll") then

			if string.find(f, "System.") then
				table.insert(t, f)
			end
		end
	end

	handle:close()
	return t
end

function linkAppReferences()
	local monoLibsPath = path.join(FoxRootDirectory, "FoxEditor", "mono", "lib", "mono", "4.5")
	local is_windows = os.istarget('windows')

	if is_windows then
		monoLibsPath = monoLibsPath:gsub("/", "\\")
	end

	-- NOTE(Peter): We HAVE to use libdirs, using the full path in links won't work
	--				this is a known issue with Visual Studio...
	libdirs { monoLibsPath }

	for k, v in ipairs(getAssemblyFiles(monoLibsPath, is_windows)) do
		print("Adding reference to: " .. v)
		links { v }
	end
end


workspace "Sandbox"
	targetdir "build"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

group "Fox"
project "Fox-ScriptCore"
	location "%{FoxRootDirectory}/Fox-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.5"

	targetdir ("%{FoxRootDirectory}/FoxEditor/Resources/Scripts")
	objdir ("%{FoxRootDirectory}/FoxEditor/Resources/Scripts/Intermediates")

	files
	{
		"%{FoxRootDirectory}/Fox-ScriptCore/Sources/**.cs",
	}

	linkAppReferences()

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group ""

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.8"

	targetname "Sandbox"
	targetdir ("%{prj.location}/Assets/Scripts/Binaries")
	objdir ("%{prj.location}/Intermediates")

	files
	{
		"Assets/**.cs",
	}

	linkAppReferences()
	links { "Fox-ScriptCore" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
