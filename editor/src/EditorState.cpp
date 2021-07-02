
#include <Core/Scene.hpp>
#include <ImGui/imgui.h>
#include <Core/Input/Input.hpp>
#include <Utils/PlatformUtils.hpp>
#include <Core/SceneSerializer.hpp>
#include <Time.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <Components/CameraComponent.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Math/Math.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Components/EntityName.hpp>

#include "EditorState.hpp"

namespace fox
{
    void EditorState::OnEnter()
    {
        fox::FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = fox::Framebuffer::Create(fbSpec);

        m_pActiveScene = new_ref<Scene>(GetApp());

        m_oEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_pActiveScene);

//            auto e1 = m_pActiveScene->NewEntity();
//            e1.add<TransformComponent>();
//            e1.add<SpriteRenderer>(glm::vec4{0.0f, 0.3f, 0.2f, 1.0f});
//            auto e2 = m_pActiveScene->NewEntity();
//            e2.add<TransformComponent>(glm::vec3{1, 1, 0});
//            e2.add<SpriteRenderer>(glm::vec4{0.2f, 0.3f, 0.5f, 1.0f});
//
//            auto m_CameraEntity = m_pActiveScene->NewEntity("Camera");
//            m_CameraEntity.add<CameraComponent>();
//            m_CameraEntity.get<CameraComponent>()->Primary = true;
//            m_CameraEntity.add<NativeScript>(Test());
    }

    void EditorState::OnExit()
    {}

    void EditorState::OnEvent(fox::Event& event)
    {
        m_CameraController.OnEvent(event);
        m_oEditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(FOX_BIND_EVENT_FN(EditorState::OnKeyPressed));

        // Because I cannot access the ImGuState instance, I set the block event here
        if (!m_bViewportFocused && !m_bViewportHovered)
        {
            ImGuiIO& io = ImGui::GetIO();
            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

    void EditorState::OnUpdate()
    {
        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
                m_oViewportSize.x > 0.0f && m_oViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (spec.Width != m_oViewportSize.x || spec.Height != m_oViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_oViewportSize.x, (uint32_t)m_oViewportSize.y);
            m_CameraController.OnResize(m_oViewportSize.x, m_oViewportSize.y);
            m_oEditorCamera.SetViewportSize(m_oViewportSize.x, m_oViewportSize.y);
            m_pActiveScene->OnViewportResize((uint32_t)m_oViewportSize.x, (uint32_t)m_oViewportSize.y);
        }

        if (m_bViewportFocused)
            m_CameraController.OnUpdate();
        m_oEditorCamera.OnUpdate();

        fox::Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        fox::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1});
        fox::RendererCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        // Update scene
        m_pActiveScene->OnUpdateEditor(m_oEditorCamera);

        auto[mx, my] = ImGui::GetMousePos();
        mx -= m_vViewportBounds[0].x;
        my -= m_vViewportBounds[0].y;
        glm::vec2 viewportSize = m_vViewportBounds[1] - m_vViewportBounds[0];

        // Flip the mouse position Y to fit with OpenGL origin axis
        // which is bottom-left
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        // Check the position of the mouse is in the viewport boundary
        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_oHoveredEntity = pixelData == -1 ? Entity() : Entity(&m_pActiveScene->GetWorld(), (EntityId)pixelData);
        }

        m_Framebuffer->Unbind();
    }

    void EditorState::OnImGui()
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

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit"))
                    m_pApp->quit();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImGui();

        ImGui::Begin("Stats");
        {
            std::string name = "None";
            if (m_oHoveredEntity)
                name = m_oHoveredEntity.get<EntityName>()->name;
            ImGui::Text("Hovered Entity: %s", name.c_str());

            auto stats = fox::Renderer2D::GetStats();
            ImGui::Text("Renderer2D Stats:");
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quads: %d", stats.QuadCount);
            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        {
            // Using a Child allow to fill all the space of the window.
            // It also alows customization
            ImGui::BeginChild("GameRender");
            auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
            auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
            auto viewportOffset = ImGui::GetWindowPos(); // Includes tab bar

            m_vViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
            m_vViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

            m_bViewportFocused = ImGui::IsWindowFocused();
            m_bViewportHovered = ImGui::IsWindowHovered();
            {
                // Get the size of the child (i.e. the whole draw size of the windows).
                ImVec2 viewportPanelSize = ImGui::GetWindowSize();
                m_oViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

                uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
                // Because I use the texture from OpenGL, I need to invert the axis from the UV.
                ImGui::Image(reinterpret_cast<void *>(textureID), viewportPanelSize, ImVec2 {0, 1}, ImVec2 {1, 0});

                // Gizmos
                Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
                if (selectedEntity && m_iGizmoType != -1)
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    float windowWidth = ImGui::GetWindowWidth();
                    float windowHeight = ImGui::GetWindowHeight();
                    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                    // Camera
                    // Runtime camera from entity
//                    auto cameraEntity = m_pActiveScene->GetPrimaryCameraEntity();
//                    const auto& camera = cameraEntity.get<CameraComponent>()->camera;
//                    const glm::mat4& cameraProjection = camera.GetProjection();
//                    glm::mat4 cameraView = glm::inverse(cameraEntity.get<TransformComponent>()->GetTransform());

                    const glm::mat4& cameraProjection = m_oEditorCamera.GetProjection();
                    glm::mat4 cameraView = m_oEditorCamera.GetViewMatrix();

                    // Entity transform
                    auto& tc = selectedEntity.get<TransformComponent>().value();
                    glm::mat4 transform = tc.GetTransform();

                    // Snapping
                    bool snap = Input::IsKeyPressed(Key::LeftControl);
                    float snapValue = 0.5f; // Snap to 0.5m for translation/scale
                    // Snap to 45 degrees for rotation
                    if (m_iGizmoType == ImGuizmo::OPERATION::ROTATE)
                        snapValue = 10.0f;

                    float snapValues[3] = { snapValue, snapValue, snapValue };

                    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                                         (ImGuizmo::OPERATION) m_iGizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                                         nullptr, snap ? snapValues : nullptr);

                    if (ImGuizmo::IsUsing())
                    {
                        glm::vec3 position, rotation, scale;
                        DecomposeTransform(transform, position, rotation, scale);

                        glm::vec3 deltaRotation = rotation - tc.rotation;
                        tc.position = position;
                        tc.rotation += deltaRotation;
                        tc.scale = scale;
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    bool EditorState::OnKeyPressed(KeyPressedEvent& e)
    {
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
                    NewScene();
            }
            break;

            case Key::O:
            {
                if (control)
                    OpenScene();
            }
            break;

            case Key::S:
            {
                if (control && shift)
                    SaveSceneAs();
            }
            break;

            // Gizmos
            case Key::Q:
                m_iGizmoType = -1;
            break;

            case Key::W:
                m_iGizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case Key::E:
                m_iGizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            case Key::R:
                m_iGizmoType = ImGuizmo::OPERATION::SCALE;
                break;
        }
    }

    void EditorState::NewScene()
    {
        m_pActiveScene = new_ref<Scene>(GetApp());
        m_SceneHierarchyPanel.SetContext(m_pActiveScene);
        m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);
    }

    void EditorState::OpenScene()
    {
        auto filepath = FileDialogs::OpenFile({"Fox Scene (*.foxscene)", "*.foxscene"});
        if (!filepath.empty()) {
            m_pActiveScene = new_ref<Scene>(GetApp());
            m_SceneHierarchyPanel.SetContext(m_pActiveScene);

            SceneSerializer serializer(m_pActiveScene);
            serializer.Deserialize(filepath);

            m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);
        }
    }

    void EditorState::SaveSceneAs()
    {
        auto filepath = FileDialogs::SaveFile({"Fox Scene (*.foxscene)", "*.foxscene"});
        if (!filepath.empty())
        {
            SceneSerializer serializer(m_pActiveScene);
            serializer.Serialize(filepath);
        }
    }
}