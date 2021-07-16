
#include <imgui.h>
#include <Scene/Scene.hpp>
#include <Input/Input.hpp>
#include <Utils/PlatformUtils.hpp>
#include <Scene/SceneSerializer.hpp>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <Math/Math.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Components/EntityName.hpp>
#include <FileSystem/Path.hpp>
#include <FileSystem/FPaths.hpp>
#include <FileSystem/FileSystem.hpp>
#include <imgui_internal.h>
#include <EditorEvent.hpp>

#include "EditorState.hpp"


void DockingToolbar(const char* name, ImGuiAxis* p_toolbar_axis, std::function<void(const ImVec2, ImGuiAxis)> func)
{
    // [Option] Automatically update axis based on parent split (inside of doing it via right-click on the toolbar)
    // Pros:
    // - Less user intervention.
    // - Avoid for need for saving the toolbar direction, since it's automatic.
    // Cons:
    // - This is currently leading to some glitches.
    // - Some docking setup won't return the axis the user would expect.
    const bool TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED = true;

    // ImGuiAxis_X = horizontal toolbar
    // ImGuiAxis_Y = vertical toolbar
    ImGuiAxis toolbar_axis = *p_toolbar_axis;

    // 1. We request auto-sizing on one axis
    // Note however this will only affect the toolbar when NOT docked.
    ImVec2 requested_size = (toolbar_axis == ImGuiAxis_X) ? ImVec2(-1.0f, 0.0f) : ImVec2(0.0f, -1.0f);
    ImGui::SetNextWindowSize(requested_size);

    // 2. Specific docking options for toolbars.
    // Currently they add some constraint we ideally wouldn't want, but this is simplifying our first implementation
    ImGuiWindowClass window_class;
    window_class.DockingAllowUnclassed = true;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar // FIXME: Will need a working Undock widget for _NoTabBar to work
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingSplitMe;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverMe;
    window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDockingOverOther;
    if (toolbar_axis == ImGuiAxis_X)
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeY;
    else
        window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoResizeX;
    ImGui::SetNextWindowClass(&window_class);

    // 3. Begin into the window
    const float font_size = ImGui::GetFontSize();
    const ImVec2 icon_size(ImFloor(font_size * 1.7f), ImFloor(font_size * 1.7f));
    ImGui::Begin(name, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    // 4. Overwrite node size
    ImGuiDockNode* node = ImGui::GetWindowDockNode();
    if (node != NULL)
    {
        // Overwrite size of the node
        ImGuiStyle& style = ImGui::GetStyle();
        const ImGuiAxis toolbar_axis_perp = (ImGuiAxis)(toolbar_axis ^ 1);
        const float TOOLBAR_SIZE_WHEN_DOCKED = style.WindowPadding[toolbar_axis_perp] * 2.0f + icon_size[toolbar_axis_perp];
        node->WantLockSizeOnce = true;
        node->Size[toolbar_axis_perp] = node->SizeRef[toolbar_axis_perp] = TOOLBAR_SIZE_WHEN_DOCKED;

        if (TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED)
            if (node->ParentNode && node->ParentNode->SplitAxis != ImGuiAxis_None)
                toolbar_axis = (ImGuiAxis)(node->ParentNode->SplitAxis ^ 1);
    }

    func(icon_size, toolbar_axis);

    // 6. Context-menu to change axis
    if (node == nullptr || !TOOLBAR_AUTO_DIRECTION_WHEN_DOCKED)
    {
        if (ImGui::BeginPopupContextWindow())
        {
            ImGui::TextUnformatted(name);
            ImGui::Separator();
            if (ImGui::MenuItem("Horizontal", "", (toolbar_axis == ImGuiAxis_X)))
                toolbar_axis = ImGuiAxis_X;
            if (ImGui::MenuItem("Vertical", "", (toolbar_axis == ImGuiAxis_Y)))
                toolbar_axis = ImGuiAxis_Y;
            ImGui::EndPopup();
        }
    }

    ImGui::End();

    // Output user stored data
    *p_toolbar_axis = toolbar_axis;
}

namespace fox
{
    EditorState::~EditorState()
    {
        m_oWatcher.reset();
        m_Framebuffer.reset();
        m_pActiveScene.reset();
    }

    void EditorState::OnEnter()
    {
        json::Value oConfigTemp;
        std::string out;

        if (fox::file::ReadFile("./editor_config.json", out))
            oConfigTemp = json::parse(out);
        if (!oConfigTemp.is_null()) {
            m_oEditorConfig = json::Value(oConfigTemp);
            fox::info("editor_config.json load successfully");
        } else
            // TODO: Project Launcher
            fox::error("Wrong configuration format for 'editor_config.json'");

        if (!m_oEditorConfig.is_null() && !m_oEditorConfig["LastOpenedProject"].is_null()) {
            // Set the project directory
            FPaths::SetProjectDir(m_oEditorConfig["LastOpenedProject"].get<std::string>());
            // Notify the content panel that the project directory changed
            m_ContentBrowserPanel.OnProjectOpen();
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
        m_pActiveScene = new_ref<Scene>(GetApp());
        m_SceneHierarchyPanel.SetContext(m_pActiveScene);

        // If we have opened a scene last time so deserialize it to the active scene
        if (!m_oEditorConfig.is_null() && !m_oEditorConfig["LastOpenedScene"].is_null())
        {
            SceneSerializer serializer(m_pActiveScene);
            serializer.Deserialize(m_oEditorConfig["LastOpenedScene"].get<std::string>());
        }

        // Bind Events
        m_OnRuntimeStart += event::MakeFunc(*this, &EditorState::OnRuntimeStart);
        m_OnRuntimeStop += event::MakeFunc(*this, &EditorState::OnRuntimeStop);

        m_SceneHierarchyPanel.SelectedEntity += event::MakeFunc(m_AnimationEditor, &AnimationEditor::OnSelectedEntityChanged);

        // InitFileWatcher();
    }

    void EditorState::OnExit()
    {
        m_oEditorConfig["LastOpenedProject"] = FPaths::ProjectDir();
        if (!fox::file::WriteFile("./editor_config.json", m_oEditorConfig.dump()))
            fox::error("Failed to write to 'editor_config.json'");
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

        // Is running the simulation
        if (m_bIsRunning)
        {
            m_pActiveScene->OnUpdateRuntime();
        }
        else
        {
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
        }

        m_Framebuffer->Unbind();
    }

    void EditorState::OnImGui()
    {
        // Dockspace
        ImGuiIO &io = ImGui::GetIO();
        {
            // Note: Switch this to true to enable dockspace
            static bool dockspaceOpen = true;
            static bool opt_fullscreen_persistant = true;
            bool opt_fullscreen = opt_fullscreen_persistant;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen) {
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
            ImGuiStyle &style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            style.WindowMinSize.x = minWinSizeX;
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project"))
                    NewProject();

                if (ImGui::MenuItem("Open Project", "Ctrl+O"))
                    OpenProject();

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit"))
                    m_pApp->quit();
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

        m_ContentBrowserPanel.OnImGui();
        m_SceneHierarchyPanel.OnImGui();
        m_AnimationEditor.OnImGui();


        ImGui::Begin("Stats");
        {
            auto stats = fox::Renderer2D::GetStats();
            ImGui::Text("Renderer2D Stats:");
            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
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
            auto viewportOffset = ImGui::GetWindowPos();

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
                    auto& tc = *selectedEntity.get<TransformComponent>();
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

        static ImGuiAxis toolbar1_axis = ImGuiAxis_X; // Your storage for the current direction.
        DockingToolbar("Toolbar1", &toolbar1_axis, [this](const ImVec2 icon_size, ImGuiAxis toolbar_axis) {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));

            if (toolbar_axis == ImGuiAxis_X)
                ImGui::SameLine();

            // If not running disable the Play button and enable Stop button
            if (m_bIsRunning)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            // Run the runtime game
            if (ImGui::Button("Play"))
            {
                // Emit that the runtime is starting
                m_OnRuntimeStart();

                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if (m_bIsRunning)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            // If not running disable the Stop button and enable Play button
            if (!m_bIsRunning)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (toolbar_axis == ImGuiAxis_X)
                ImGui::SameLine();

            // Stop the runtime game
            if (ImGui::Button("Stop"))
            {
                // Emit that the runtime is stopped
                m_OnRuntimeStop();

                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (!m_bIsRunning)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            ImGui::PopStyleVar(2);
        });

        ImGui::End();
    }

    ////////////////////////////////////////////
    /// Events
    ////////////////////////////////////////////
    void EditorState::OnEvent(fox::Event& event)
    {
        m_CameraController.OnEvent(event);
        m_oEditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(FOX_BIND_EVENT_FN(EditorState::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(FOX_BIND_EVENT_FN(EditorState::OnMouseButtonPressed));

        // Because I cannot access the ImGuState instance, I set the block event here
        if (!m_bViewportFocused && !m_bViewportHovered)
        {
            ImGuiIO& io = ImGui::GetIO();
            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
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
                else if (control)
                    SaveScene();
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
        return false;
    }

    bool EditorState::OnMouseButtonPressed(MouseButtonPressedEvent& e)
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
        file::WriteFile(hpp_filepath, "\n#include <Components/NativeScript.hpp>\n"
                                      "\nclass "+ strName +" : public ScriptableBehaviour\n{\n"
                                         "public:\n"
                                         "protected:\n"
                                          "    void OnStart() override;\n"
                                          "    void on_update() override;\n"
                                         "private:\n"
                                         "};\n\n"
                                         "REGISTER_SCRIPT("+ strName +");");

        // Create the CPP Class
        file::WriteFile(cpp_filepath, "\n#include <FoxEngine.hpp>\n"
                                      "#include \"" + strName + ".hpp\""
                                    "\n\n"
                                    "\nvoid " + strName + "::OnStart()\n{\n}\n"
                                    "\nvoid " + strName + "::on_update()\n{\n}\n"
         );

        fox::info("Create Cpp File in '%'", cpp_filepath);
        fox::info("Create Hpp File in '%'", hpp_filepath);
    }

    ////////////////////////////////////////////
    /// Asset File
    ////////////////////////////////////////////
    void EditorState::NewScript()
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
    /// Project File
    ////////////////////////////////////////////
    void EditorState::NewProject()
    {
        auto filepath = FileDialogs::SaveFile({"Fox Project (*.foxproject)", "*.foxproject"});
        if (!filepath.empty())
        {
            Path path(filepath);
            const std::string& project_root = path.parent_path().string();
            const std::string& sample_scene = project_root + "/assets/Scenes/Sample.foxscene";

            std::filesystem::create_directories(project_root + "/assets/Scenes/");
            file::WriteFile(filepath, "{\n}");
            file::WriteFile(sample_scene, "");

            m_pActiveScene = new_ref<Scene>(GetApp());
            m_SceneHierarchyPanel.SetContext(m_pActiveScene);

            SceneSerializer serializer(m_pActiveScene);
            serializer.Deserialize(sample_scene);

            m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);

            FPaths::SetProjectDir(path.parent_path().string());
            m_ContentBrowserPanel.OnProjectOpen();
        }
    }

    void EditorState::OpenProject()
    {
        auto filepath = FileDialogs::OpenFile({"Fox Project (*.foxproject)", "*.foxproject"});
        if (!filepath.empty()) {
            m_pActiveScene = new_ref<Scene>(GetApp());
            m_SceneHierarchyPanel.SetContext(m_pActiveScene);

            Path path(filepath);
            FPaths::SetProjectDir(path.parent_path().string());
            m_ContentBrowserPanel.OnProjectOpen();
        }
    }

    ////////////////////////////////////////////
    /// Scene File
    ////////////////////////////////////////////
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

            m_oEditorConfig["LastOpenedScene"] = filepath;

            m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);
        }
    }

    void EditorState::SaveScene()
    {
        SceneSerializer serializer(m_pActiveScene);
        serializer.Serialize(m_oEditorConfig["LastOpenedScene"].get<std::string>());
    }

    void EditorState::SaveSceneAs()
    {
        auto filepath = FileDialogs::SaveFile({"Fox Scene (*.foxscene)", "*.foxscene"});
        if (!filepath.empty())
        {
            m_oEditorConfig["LastOpenedScene"] = filepath;

            SceneSerializer serializer(m_pActiveScene);
            serializer.Serialize(filepath);
        }
    }

    void EditorState::OnRuntimeStart()
    {
        m_bIsRunning = true;

        // Create the meta directory
        std::filesystem::create_directories("metas");
        // Save the scene in a temp file
        SceneSerializer serializer(m_pActiveScene);
        serializer.Serialize("metas/active_scene.foxscene.meta");

        m_pActiveScene->OnStartRuntime();
    }

    void EditorState::OnRuntimeStop()
    {
        m_bIsRunning = false;

        // Create a new scene and the context of the scene panel
        m_pActiveScene = new_ref<Scene>(GetApp());
        m_SceneHierarchyPanel.SetContext(m_pActiveScene);

        // Reload the scene from the temp file
        SceneSerializer serializer(m_pActiveScene);
        serializer.Deserialize("metas/active_scene.foxscene.meta");

        // Send the viewport event to set the viewport of the games cameras properly
        m_pActiveScene->OnViewportResize(m_oViewportSize.x, m_oViewportSize.y);
    }

    void EditorState::InitFileWatcher()
    {
        // Create a FileWatcher instance that will check the current folder for changes every 5 seconds
        m_oWatcher = new_scope<FileWatcher>(FPaths::AssetsDir(), std::chrono::milliseconds(5000));

        // Start monitoring a folder for changes and (in case of changes)
        // run a user provided lambda function
        m_oWatcher->start([] (std::string path_to_watch, FileStatus status) {
            // Process only regular files, all other file types are ignored
            if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
                return;
            }

            switch(status)
            {
                case FileStatus::created:
                    std::cout << "File created: " << path_to_watch << '\n';
                    break;
                case FileStatus::modified:
                    std::cout << "File modified: " << path_to_watch << '\n';
                    break;
                case FileStatus::erased:
                    std::cout << "File erased: " << path_to_watch << '\n';
                    break;
                default:
                    std::cout << "Error! Unknown file status.\n";
            }
        });
    }
}