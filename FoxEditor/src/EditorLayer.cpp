
#include "Scene/Scene.hpp"
#include <imgui.h>
#include <Core/Input/Input.hpp>
#include <Utils/PlatformUtils.hpp>
#include "Scene/SceneSerializer.hpp"
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <Math/Math.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Utils/Path.hpp>
#include <Utils/FileSystem.hpp>
#include <imgui_internal.h>
#include "EditorEvent.hpp"

#include "EditorLayer.hpp"
#include "Panels/PropertyPanel.hpp"
#include "Panels/StatsPanel.hpp"
#include "Panels/SettingsPanel.hpp"
#include "Panels/ConsolePanel.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Core/Project.hpp"
#include "Core/ProjectSerializer.hpp"

namespace fox
{
//    FileWatcher fw {"./SandboxProject/Assets/", std::chrono::milliseconds(2000)};

    EditorLayer::~EditorLayer()
    {
        m_Framebuffer.Reset();
        m_pActiveScene.Reset();
    }

    void EditorLayer::OnAttach()
    {
        // Load Textures for Editor
        m_IconPlay = Texture2D::Create("Resources/PlayButton.png");
        m_IconSimulate = Texture2D::Create("Resources/SimulateButton.png");
        m_IconStop = Texture2D::Create("Resources/StopButton.png");

        // Create Panels
        m_Panels.emplace_back(new_ref<PropertyPanel>());
        m_Panels.emplace_back(new_ref<ContentBrowserPanel>());
        m_Panels.emplace_back(new_ref<StatsPanel>());
        m_Panels.emplace_back(new_ref<SettingsPanel>());
        m_Panels.emplace_back(new_ref<ConsolePanel>());

        // Bind Signals
        m_OnImGuiRenderEvent.Bind<SceneHierarchyPanel, &SceneHierarchyPanel::OnImGui>(m_SceneHierarchyPanel);
        for (Ref<Panel> panel : m_Panels)
        {
            m_OnImGuiRenderEvent.Bind<Panel, &Panel::OnImGui>(*panel);
            if (panel->EnableOverlay)
                m_OnOverlayRenderEvent.Bind<Panel, &Panel::OnOverlayRender>(*panel);
        }

        // Create frame buffer
        fox::FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = fox::Framebuffer::Create(fbSpec);

        // Init Editor Camera
        m_oEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

        // Create a new Scene
        m_pEditorScene = new_ref<Scene>();
        m_pActiveScene = m_pEditorScene;
        event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));

//        // Open a scene if provided in cmd arguments
//        auto commandLineArgs = Application::Get().GetSpecs().CommandLineArgs;
//        if (commandLineArgs.Count > 1)
//        {
//            auto sceneFilePath = commandLineArgs[1];
//            OpenScene(sceneFilePath);
//        }

        if (!m_UserPrefs->StartupProject.empty())
            OpenProject(m_UserPrefs->StartupProject);
//        else
//            FOX_CORE_VERIFY(false, "No project provided!");
    }

    void EditorLayer::OnDetach()
    {
        CloseProject(false);
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        m_pActiveScene->OnViewportResize((uint32_t)m_oViewportSize.x, (uint32_t)m_oViewportSize.y);

        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
                m_oViewportSize.x > 0.0f && m_oViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (spec.Width != m_oViewportSize.x || spec.Height != m_oViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_oViewportSize.x, (uint32_t)m_oViewportSize.y);
            m_CameraController.OnResize(m_oViewportSize.x, m_oViewportSize.y);
            m_oEditorCamera.SetViewportSize(m_oViewportSize.x, m_oViewportSize.y);
        }

        fox::Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        fox::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1});
        fox::RendererCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if (m_bViewportFocused)
                    m_CameraController.OnUpdate((float)ts);

                m_oEditorCamera.OnUpdate();
                m_pActiveScene->OnUpdateEditor(m_oEditorCamera);
                break;
            }
            case SceneState::Simulate:
            {
                m_oEditorCamera.OnUpdate();
                m_pActiveScene->OnUpdateSimulation(m_oEditorCamera, ts);
                break;
            }
            case SceneState::Play:
            {
                m_pActiveScene->OnUpdateRuntime(ts);
                break;
            }
        }

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
            m_oHoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_pActiveScene.Raw());
        }

        OnOverlayRender();
        m_Framebuffer->Unbind();
    }

    void EditorLayer::BeginDockspace()
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
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
    }

    void EditorLayer::EndDockspace()
    {
        ImGui::End();
    }

    void EditorLayer::OnImGuiRender()
    {
        BeginDockspace();

        UI_MenuBar();

        m_OnImGuiRenderEvent.Invoke();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        {
            // Using a Child allow to fill all the space of the window.
            // It also allows customization
            ImGui::BeginChild("GameRender");
            {
                auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
                auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
                auto viewportOffset = ImGui::GetWindowPos();

                m_vViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
                m_vViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

                m_bViewportFocused = ImGui::IsWindowFocused();
                m_bViewportHovered = ImGui::IsWindowHovered();
                Application::Get().GetImGuiLayer()->BlockEvents(!m_bViewportHovered);

                // Get the size of the child (i.e. the whole draw size of the windows).
                ImVec2 viewportPanelSize = ImGui::GetWindowSize();
                m_oViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

                uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
                // Because I use the texture from OpenGL, I need to invert the axis from the UV.
                ImGui::Image(reinterpret_cast<void *>(textureID), viewportPanelSize, ImVec2 {0, 1}, ImVec2 {1, 0});

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                    {
                        const char* path = (const char*)payload->Data;
                        auto filepath = Project::AssetsDir() / path;

                        if (filepath.extension() == ".foxscene")
                        {
                            OpenScene(Project::AssetsDir() / path);
                        }

                        if (filepath.extension() == ".foxprefab")
                        {
                            Entity entity = EntitySerializer::DeserializeEntityAsPrefab(filepath.c_str(), *m_pActiveScene);
                            event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(entity));
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Gizmos
                Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
                if (selectedEntity && m_iGizmoType != -1 && !Input::IsKeyPressed(Key::LeftAlt))
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    float windowWidth = ImGui::GetWindowWidth();
                    float windowHeight = ImGui::GetWindowHeight();
                    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                    // Camera
                    const glm::mat4& cameraProjection = m_oEditorCamera.GetProjection();
                    glm::mat4 cameraView = m_oEditorCamera.GetViewMatrix();

                    // Entity transform
                    auto& tc = selectedEntity.get<TransformComponent>();
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

                        glm::vec3 deltaRotation = rotation - tc.GetRotation();
                        tc.position = position;
                        tc.SetRotation(tc.GetRotation() + deltaRotation);
                        tc.scale = scale;
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopStyleVar();

        UI_Toolbar();
        EndDockspace();
    }

    void EditorLayer::UI_MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // if (ImGui::MenuItem("New Project"))
                //     NewProject();

                 if (ImGui::MenuItem("Open Project", "Ctrl+O"))
                     OpenProject();

                if (ImGui::MenuItem("Save Project"))
                    SaveProject();

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Assets"))
            {
                if (ImGui::MenuItem("Create Script"))
                    NewScript();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void EditorLayer::UI_Toolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        {
            bool toolbarEnabled = (bool)m_pActiveScene;

            ImVec4 tintColor = ImVec4(1, 1, 1, 1);
            if (!toolbarEnabled)
                tintColor.w = 0.5f;

            float size = ImGui::GetWindowHeight() - 4.0f;
            {
                Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
                ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
                if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
                {
                    if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                        OnScenePlay();
                    else if (m_SceneState == SceneState::Play)
                        OnSceneStop();
                }
            }
            ImGui::SameLine();
            {
                Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
                //ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
                if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
                {
                    if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
                        OnSceneSimulate();
                    else if (m_SceneState == SceneState::Simulate)
                        OnSceneStop();
                }
            }
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnOverlayRender()
    {
        if (m_SceneState == SceneState::Play)
        {
            Entity camera = m_pActiveScene->GetPrimaryCameraEntity();
            if (!camera)
                return;

            Renderer2D::BeginScene(camera.get<CameraComponent>().camera, camera.get<TransformComponent>().GetTransform());
        }
        else
        {
            Renderer2D::BeginScene(m_oEditorCamera);
        }

        m_OnOverlayRenderEvent.Invoke();

        // Draw selected entity outline
        if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
        {
            const TransformComponent& transform = selectedEntity.get<TransformComponent>();
            Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        }

        Renderer2D::EndScene();
    }

    ////////////////////////////////////////////
    /// Events
    ////////////////////////////////////////////
    void EditorLayer::OnEvent(fox::Event& event)
    {
        m_CameraController.OnEvent(event);
        if (m_SceneState == SceneState::Edit)
        {
            m_oEditorCamera.OnEvent(event);
        }

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(FOX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(FOX_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
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
                if (control)
                {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }
            }
            break;

            // Scene Commands
            case Key::D:
            {
                if (control)
                    OnDuplicateEntity();

                break;
            }

            // Gizmos
            case Key::Q:
                if (!ImGuizmo::IsUsing())
                    m_iGizmoType = -1;
                break;
            break;

            case Key::W:
            {
                if (!ImGuizmo::IsUsing())
                    m_iGizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case Key::E:
            {
                if (!ImGuizmo::IsUsing())
                    m_iGizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case Key::R:
            {
                if (control)
                {
                    ScriptEngine::ReloadAppDomain();
                }
                else
                {
                    if (!ImGuizmo::IsUsing())
                        m_iGizmoType = ImGuizmo::OPERATION::SCALE;
                }
                break;
            }
        }
        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_bViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
                m_SceneHierarchyPanel.SetSelectedEntity(m_oHoveredEntity);
        }
        return false;
    }

    void CreateClass(const std::string& path, const std::string& strName)
    {
        const std::string& hpp_filepath = path + "/" + strName + ".hpp";
        const std::string& cpp_filepath = path + "/" + strName + ".cpp";

        // Create the HPP Class
        FileSystem::WriteFile(hpp_filepath, "\n#include <Components/NativeScript.hpp>\n"
                                      "\nclass "+ strName +" : public ScriptableBehaviour\n{\n"
                                         "public:\n"
                                         "protected:\n"
                                          "    void OnStart() override;\n"
                                          "    void on_update() override;\n"
                                         "private:\n"
                                         "};\n\n"
                                         "REGISTER_SCRIPT("+ strName +");");

        // Create the CPP Class
        FileSystem::WriteFile(cpp_filepath, "\n#include <FoxEngine.hpp>\n"
                                      "#include \"" + strName + ".hpp\""
                                    "\n\n"
                                    "\nvoid " + strName + "::OnStart()\n{\n}\n"
                                    "\nvoid " + strName + "::on_update()\n{\n}\n"
         );

        FOX_CORE_INFO("Create Cpp File in '%'", cpp_filepath);
        FOX_CORE_INFO("Create Hpp File in '%'", hpp_filepath);
    }

    ////////////////////////////////////////////
    /// Asset File
    ////////////////////////////////////////////
    void EditorLayer::NewScript()
    {
        auto filepath = FileDialogs::SaveFile({"C++ (*.cpp)", "*.cpp"});
        if (!filepath.empty())
        {
            Path path(filepath);
            const std::string& root_path = path.parent_path().string();
            const std::string& filename = path.basename();

            CreateClass(root_path, filename);
        }
    }

    ////////////////////////////////////////////
    /// Project File    // DO NOT DELETE -----
    ////////////////////////////////////////////
    // void EditorLayer::NewProject()
    // {
    //     auto filepath = FileDialogs::SaveFile({"Fox Project (*.foxproject)", "*.foxproject"});
    //     if (!filepath.empty())
    //     {
    //         Path path(filepath);
    //         const std::string& project_root = path.parent_path().string();
    //         const std::string& sample_scene = project_root + "/assets/Scenes/Sample.foxscene";

    //         std::filesystem::create_directories(project_root + "/assets/Scenes/");
    //         file::WriteFile(filepath, "{\n}");
    //         file::WriteFile(sample_scene, "");

    //         m_pActiveScene = new_ref<Scene>(GetApp());
    //         m_SceneHierarchyPanel.SetContext(m_pActiveScene);

    //         SceneSerializer serializer(m_pActiveScene);
    //         serializer.Deserialize(sample_scene);

    //         m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);

    //         FPaths::SetProjectDir(path.parent_path().string());
    //         m_ContentBrowserPanel.OnProjectOpen();
    //     }
    // }

     void EditorLayer::OpenProject()
     {
         auto filepath = FileDialogs::OpenFile({"Fox Project (*.foxproject)", "*.foxproject"});
         if (!filepath.empty()) {
             OpenProject(filepath);
         }
     }

    void EditorLayer::OpenProject(const std::string& filepath)
    {
        if (!FileSystem::Exists(filepath))
        {
            FOX_CORE_ERROR("Tried to open a project that doesn't exist. Project path: %", filepath);
            return;
        }

        if (Project::GetActive())
            CloseProject();

        Ref<Project> project = Ref<Project>::Create();
        ProjectSerializer serializer(project);
        serializer.Deserialize(filepath);
        Project::SetActive(project);

        auto appAssemblyPath = Project::ScriptModuleFilePath();
        if (!appAssemblyPath.empty() && FileSystem::Exists(appAssemblyPath))
            ScriptEngine::LoadAppAssembly(appAssemblyPath);
        else
            FOX_WARN("No C# assembly has been provided in the Project Settings, or it wasn't found.");

        if (!project->GetConfig().StartScene.empty())
            OpenScene((Project::AssetsDir() / project->GetConfig().StartScene).string());
        else
            NewScene();

        event::EventSystem::Get().Emit(OnProjectChangeEvent());

//        // Reset cameras
//        m_oEditorCamera = EditorCamera(45.0f, 1280.0f, 720.0f, 0.1f, 1000.0f);
//        m_SecondEditorCamera = EditorCamera(45.0f, 1280.0f, 720.0f, 0.1f, 1000.0f);
    }

    void EditorLayer::CloseProject(bool unloadProject)
    {
        SaveProject();

//        m_PanelManager->SetSceneContext(nullptr);
//        ScriptEngine::UnloadAppAssembly();
        ScriptEngine::SetSceneContext(nullptr);
//        m_ViewportRenderer->SetScene(nullptr);
//        m_SecondViewportRenderer->SetScene(nullptr);
        m_pActiveScene = nullptr;

        // Check that m_EditorScene is the last one (so setting it null here will destroy the scene)
        FOX_CORE_ASSERT(m_pEditorScene->GetRefCount() == 1, "Scene will not be destroyed after project is closed - something is still holding scene refs!");
        m_pEditorScene = nullptr;

        if (unloadProject)
            Project::SetActive(nullptr);
    }

    void EditorLayer::SaveProject()
    {
        if (!Project::GetActive())
            FOX_CORE_ASSERT(false); // TODO

        Ref<Project> project = Project::GetActive();
        ProjectSerializer serializer(project);
        serializer.Serialize(project->GetConfig().ProjectDirectory + "/" + project->GetConfig().ProjectFileName);
    }


    ////////////////////////////////////////////
    /// Scene File
    ////////////////////////////////////////////
    void EditorLayer::NewScene()
    {
        m_pActiveScene = new_ref<Scene>();
        m_EditorScenePath = std::filesystem::path();

        event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        if (path.extension().string() != ".foxscene")
        {
            FOX_CORE_WARN("Could not load % - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = new_ref<Scene>();
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))
        {
            m_pEditorScene = newScene;
            m_pActiveScene = m_pEditorScene;
            m_EditorScenePath = path;
            event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));
        }
    }

    void EditorLayer::OpenScene()
    {
        auto filepath = FileDialogs::OpenFile({"Fox Scene (*.foxscene)", "*.foxscene"});
        if (!filepath.empty()) {
            OpenScene(filepath);
        }
    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
        {
            SceneSerializer serializer(m_pActiveScene);
            serializer.Serialize(m_EditorScenePath);
            return;
        }
        SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        auto filepath = FileDialogs::SaveFile({"Fox Scene (*.foxscene)", "*.foxscene"});
        if (!filepath.empty())
        {
            SceneSerializer serializer(m_pActiveScene);
            serializer.Serialize(filepath);

            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::OnScenePlay()
    {
        m_SceneState = SceneState::Play;

        m_pActiveScene = Scene::Copy(m_pEditorScene);
        m_pActiveScene->OnRuntimeStart();

        event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));
    }

    void EditorLayer::OnSceneSimulate()
    {
        if (m_SceneState == SceneState::Play)
            OnSceneStop();

        m_SceneState = SceneState::Simulate;

        m_pActiveScene = Scene::Copy(m_pEditorScene);
        m_pActiveScene->OnSimulationStart();

        event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));
    }

    void EditorLayer::OnSceneStop()
    {
        FOX_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

        if (m_SceneState == SceneState::Play)
            m_pActiveScene->OnRuntimeStop();
        else if (m_SceneState == SceneState::Simulate)
            m_pActiveScene->OnSimulationStop();

        m_SceneState = SceneState::Edit;
        m_pActiveScene = m_pEditorScene;
        event::EventSystem::Get().Emit(OnContextChangeEvent(m_pActiveScene));
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
            m_pEditorScene->DuplicateEntity(selectedEntity);
    }
}
