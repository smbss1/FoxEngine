

#include <Core/Input/InputManager.hpp>
#include <Core/Logger/Logger.hpp>
#include <Core/State.hpp>
#include <Core/Managers/StateMachine.hpp>
#include <Core/Managers/PluginManager.hpp>
#include <Renderer/EditorCamera.hpp>
#include <Components/CameraComponent.hpp>
#include <Core/Input/Input.hpp>
#include "EditorApp.hpp"
#include "Animator.hpp"
#include "ImGui/imgui.h"
#include "Renderer/Framebuffer.hpp"
#include "SceneHierarchyPanel.hpp"

#include "ScriptableBehaviour.hpp"
#include "NativeScript.hpp"

namespace fox
{
    class Test : public ScriptableBehaviour
    {
        float value;
    public:
        ~Test() override = default;

    protected:
        void on_create() override
        {
            auto& translation = get_component<TransformComponent>()->position;
            translation.x = rand() % 10 - 5.0f;
        }

        void on_update() override
        {
            auto& translation = get_component<TransformComponent>()->position;

            float speed = 5.0f;

            if (Input::IsKeyPressed(Key::A))
                translation.x -= speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::D))
                translation.x += speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::W))
                translation.y += speed * Time::delta_time;
            if (Input::IsKeyPressed(Key::S))
                translation.y -= speed * Time::delta_time;
        }

        void on_destroy() override { }
        int i = 0;
    };

    class EditorLayer : public State
    {
    public:
        EditorLayer()
        : m_CameraController(1280.f / 720.f)
        , State("Editor") {}

        ~EditorLayer() override = default;

        void OnEnter() override
        {
            fox::FramebufferSpecification fbSpec;
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            m_Framebuffer = fox::Framebuffer::Create(fbSpec);

            m_pActiveScene = new_ref<Scene>(GetApp());

//            m_EditorCamera = graphic_ctx.create_editor_camera();
            m_SceneHierarchyPanel.SetContext(m_pActiveScene);

            m_pActiveScene->NewEntity()
                    .add<TransformComponent>()
                    .add<SpriteRenderer>(glm::vec4{0.0f, 0.3f, 0.2f, 1.0f});

            auto m_CameraEntity = m_pActiveScene->NewEntity("Camera");
            m_CameraEntity.add<CameraComponent>();
            m_CameraEntity.get<CameraComponent>()->Primary = true;
            m_CameraEntity.add<NativeScript>(Test());
        }

        void OnExit() override
        {}

        void OnEvent(fox::Event& event) override
        {
            m_CameraController.OnEvent(event);
        }

        void OnUpdate() override
        {
            if (m_bViewportFocused)
                m_CameraController.OnUpdate();

            fox::Renderer2D::ResetStats();

            m_Framebuffer->Bind();

            fox::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1});
            fox::RendererCommand::Clear();

            m_pActiveScene->OnUpdateRuntime();

//            m_EditorCamera->OnUpdate(*m_pApp);
            m_Framebuffer->Unbind();
        }

        void OnImGui() override
        {
            // Note: Switch this to true to enable dockspace
            static bool dockspaceOpen = true;
            static bool opt_fullscreen_persistant = true;
            bool opt_fullscreen = opt_fullscreen_persistant;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
            ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO &io = ImGui::GetIO();
            ImGuiStyle &style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            style.WindowMinSize.x = minWinSizeX;

            m_SceneHierarchyPanel.OnImGui();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            ImGui::Begin("Viewport");
            {
                // Using a Child allow to fill all the space of the window.
                // It also alows customization
                ImGui::BeginChild("GameRender");
                m_bViewportFocused = ImGui::IsWindowFocused();
                {
                    // Get the size of the child (i.e. the whole draw size of the windows).
                    ImVec2 viewportPanelSize = ImGui::GetWindowSize();

                    if (m_oViewportSize != *((Vec2 *) &viewportPanelSize)) {
                        m_oViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
                        m_Framebuffer->Resize(viewportPanelSize.x, viewportPanelSize.y);
//                        m_EditorCamera->SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
                        m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
                        m_pActiveScene->OnViewportResize(viewportPanelSize.x, viewportPanelSize.y);
                    }

                    uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
                    // Because I use the texture from OpenGL, I need to invert the V from the UV.
                    ImGui::Image(reinterpret_cast<void *>(textureID), viewportPanelSize, ImVec2 {0, 1}, ImVec2 {1, 0});
                }
                ImGui::EndChild();
            }
            ImGui::End();
            ImGui::PopStyleVar();


            ImGui::Begin("Stats");
            {
                auto stats = fox::Renderer2D::GetStats();
                ImGui::Text("Renderer2D Stats:");
                ImGui::Text("Delta time: %f", Time::delta_time);
                ImGui::Text("Draw Calls: %d", stats.DrawCalls);
                ImGui::Text("Quads: %d", stats.QuadCount);
                ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
                ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
            }
            ImGui::End();

            ImGui::End();
        }

    private:
        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        fox::ref<Framebuffer> m_Framebuffer;
        bool m_bViewportFocused = false;
        fox::ref<EditorCamera> m_EditorCamera;
        SceneHierarchyPanel m_SceneHierarchyPanel;
        ref<Scene> m_pActiveScene;
    };
}

EditorApp::EditorApp(int argc, char** argv) : fox::Application(argc, argv)
{
}

EditorApp::~EditorApp() { }

void EditorApp::init()
{
    fox::StateMachine& sceneManager = get<fox::StateMachine>().value();
    sceneManager.PushState(new fox::EditorLayer());
}

fox::Application* CreateApp(int argc, char** argv)
{
    return new EditorApp(argc, argv);
}