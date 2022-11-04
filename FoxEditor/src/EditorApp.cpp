
#include <FoxEngine.hpp>
#include <Core/EntryPoint.hpp>

#include <Core/State.hpp>
#include <Renderer/EditorCamera.hpp>
#include "EditorLayer.hpp"
#include "Core/UserPreferences.hpp"
#include "Utils/FileSystem.hpp"
#include <Core/Input/Input.hpp>

class EditorApp : public fox::Application
{
    public:
        explicit EditorApp(const fox::ApplicationSpecification& specification);
        ~EditorApp() override = default;

    private:
        std::string m_ProjectPath;
        fox::Ref<fox::UserPreferences> m_UserPrefs;
};

EditorApp::EditorApp(const fox::ApplicationSpecification& specification) : fox::Application(specification), m_UserPrefs(fox::new_ref<fox::UserPreferences>())
{
    // Open a scene if provided in cmd arguments
    auto commandLineArgs = Application::Get().GetSpecs().CommandLineArgs;
    if (commandLineArgs.Count > 1)
        m_ProjectPath = commandLineArgs[1];
    else
        m_ProjectPath = "SandboxProject/Sandbox.foxproj";

    // User Preferences
    {
        if (!std::filesystem::exists("UserPreferences.yaml"))
            m_UserPrefs->Save("UserPreferences.yaml");
        else
            m_UserPrefs->ConstructFrom("UserPreferences.yaml");

        if (!m_ProjectPath.empty())
            m_UserPrefs->StartupProject = m_ProjectPath;
        else if (!m_UserPrefs->StartupProject.empty())
            m_ProjectPath = m_UserPrefs->StartupProject;
    }

    // Update the FOX_DIR environment variable every time we launch
    {
        std::filesystem::path workingDirectory = std::filesystem::current_path();
        if (workingDirectory.stem().string() == "FoxEditor")
            workingDirectory = workingDirectory.parent_path();
//        fox::FileSystem::SetEnvironmentVariable("FOX_DIR", workingDirectory.string());
        m_UserPrefs->EnginePath = workingDirectory.string();
        m_UserPrefs->Save("UserPreferences.yaml");
    }

    PushLayer(new fox::EditorLayer(m_UserPrefs));
}

fox::Application* fox::CreateApp(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "Fox Editor";
    spec.CommandLineArgs = args;
    spec.ScriptSetting.CoreAssemblyPath = "Resources/Scripts/Fox-ScriptCore.dll";
    spec.ScriptSetting.EnableDebugging = false;

    return new EditorApp(spec);
}
