
#include <FoxEngine.hpp>
#include <Core/EntryPoint.hpp>

#include <Core/State.hpp>
#include <Renderer/EditorCamera.hpp>
#include "EditorLayer.hpp"
#include "Core/UserPreferences.hpp"
#include <Core/Input/Input.hpp>
// #include "ScriptableBehaviour.hpp"

class EditorApp : public fox::Application
{
    public:
        EditorApp(const fox::ApplicationSpecification& specification);
        ~EditorApp() override = default;


    private:
        std::string m_ProjectPath;
        fox::Ref<fox::UserPreferences> m_UserPrefs;
};


namespace fox
{
    // class Test : public ScriptableBehaviour
    // {
    //     float value;
    // public:
    //     ~Test() override = default;

    // protected:
    //     void on_create() override
    //     {
    //         auto& translation = get_component<TransformComponent>()->position;
    //         translation.x = rand() % 10 - 5.0f;
    //     }

    //     void on_update() override
    //     {
    //         auto& translation = get_component<TransformComponent>()->position;

    //         float speed = 5.0f;

    //         if (Input::IsKeyPressed(KeyCode::A))
    //             translation.x -= speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::D))
    //             translation.x += speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::W))
    //             translation.y += speed * Time::delta_time;
    //         if (Input::IsKeyPressed(KeyCode::S))
    //             translation.y -= speed * Time::delta_time;
    //     }

    //     void on_destroy() override { }
    //     int i = 0;
    // };
}

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
        fox::UserPreferencesSerializer serializer(m_UserPrefs);
        if (!std::filesystem::exists("UserPreferences.yaml"))
            serializer.Serialize("UserPreferences.yaml");
        else
            serializer.Deserialize("UserPreferences.yaml");

        if (!m_ProjectPath.empty())
            m_UserPrefs->StartupProject = m_ProjectPath;
        else if (!m_UserPrefs->StartupProject.empty())
            m_ProjectPath = m_UserPrefs->StartupProject;
    }

    PushLayer(new fox::EditorLayer(m_UserPrefs));
}

fox::Application* fox::CreateApp(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "Fox Editor";
    spec.CommandLineArgs = args;

    return new EditorApp(spec);
}
