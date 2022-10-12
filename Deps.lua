-- Fox Dependencies

IncludeDir = {}
IncludeDir["yaml_cpp"] = "%{wks.location}/Fox/vendor/yaml-cpp/include"
IncludeDir["box2d"] = "%{wks.location}/Fox/vendor/box2d/include"
IncludeDir["glfw"] = "%{wks.location}/Fox/vendor/glfw/include"
IncludeDir["glad"] = "%{wks.location}/Fox/vendor/glad"
IncludeDir["imgui"] = "%{wks.location}/Fox/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Fox/vendor/ImGuizmo/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Fox/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Fox/vendor/entt/include"
IncludeDir["mono"] = "%{wks.location}/Fox/vendor/monos/include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/Fox/vendor/monos"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-2.0.so"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"