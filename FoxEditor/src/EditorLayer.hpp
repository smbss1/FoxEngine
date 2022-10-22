/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_STATE_HPP_
#define FOX_EDITOR_STATE_HPP_

#include "Utils/Vec2.hpp"
// #include "json.hpp"

#include "FoxEngine.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Renderer/EditorCamera.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Events/KeyEvent.hpp"

#include "Core/Layer.hpp"
#include "Events/Signal.hpp"
#include "Panels/Panel.hpp"
#include "Core/UserPreferences.hpp"

class RuntimeStartEvent;
class RuntimeStopEvent;

namespace fox
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(Ref<UserPreferences> UserPrefs)
                : m_CameraController(1280.f / 720.f), m_UserPrefs(UserPrefs), Layer("Editor")
        {
        }

        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;

    private:

        void BeginDockspace();
        void EndDockspace();
        void UI_MenuBar();

        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        // void NewProject();
        void OpenProject();
        void OpenProject(const std::string& filepath);
        void CloseProject(bool unloadProject = true);
        void SaveProject();

        void NewScene();
        void OpenScene(const std::filesystem::path& path);
        void OpenScene();
        void SaveScene();
        void SaveSceneAs();

        void NewScript();

        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();

        void OnDuplicateEntity();

        /**
         * @brief Start init the file watcher for hot reload asset
         */
        // void InitFileWatcher();

        // UI Panel
        void UI_Toolbar();
        void OnOverlayRender();

    private:
        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        fox::Ref<Framebuffer> m_Framebuffer;
        bool m_bViewportFocused = false;
        bool m_bViewportHovered = false;
        Ref<Scene> m_pActiveScene;
        Ref<Scene> m_pEditorScene;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        std::vector<Ref<Panel>> m_Panels;

        EditorCamera m_oEditorCamera;

        int m_iGizmoType = -1;

        glm::vec2 m_vViewportBounds[2];
        Entity m_oHoveredEntity;
        std::string m_EditorScenePath;

        fox::Ref<fox::UserPreferences> m_UserPrefs;
        // scope<FileWatcher> m_oWatcher;
        enum class SceneState
        {
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_SceneState = SceneState::Edit;

        // Editor resources
        Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;

        // Events Defs
        SIGNAL(OnImGuiRenderEvent);

    public:
        // Events
        OnImGuiRenderEvent m_OnImGuiRenderEvent;
    };
}

#endif /* !FOX_EDITOR_STATE_HPP_ */
