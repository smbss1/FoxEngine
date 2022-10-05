/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_STATE_HPP_
#define FOX_EDITOR_STATE_HPP_

#include "Utils/Vec2.hpp"
#include "json.hpp"

#include "FoxEngine.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Renderer/EditorCamera.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Events/KeyEvent.hpp"

#include "Core/Layer.hpp"

class RuntimeStartEvent;
class RuntimeStopEvent;

namespace fox
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer()
                : m_CameraController(1280.f / 720.f), Layer("Editor")
        {}

        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;

    private:

        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        // void NewProject();
        // void OpenProject();

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
        bool m_bIsRunning = false;

        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        fox::ref<Framebuffer> m_Framebuffer;
        bool m_bViewportFocused = false;
        bool m_bViewportHovered = false;
        ref<Scene> m_pActiveScene;
        ref<Scene> m_pEditorScene;
        ContentBrowserPanel m_ContentBrowserPanel;
        SceneHierarchyPanel m_SceneHierarchyPanel;

        EditorCamera m_oEditorCamera;

        int m_iGizmoType = -1;

        glm::vec2 m_vViewportBounds[2];

        Entity m_oHoveredEntity;

        std::string m_EditorScenePath;

        // json::Value m_oEditorConfig;
        // scope<FileWatcher> m_oWatcher;
        bool m_ShowPhysicsColliders = false;
        enum class SceneState
        {
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_SceneState = SceneState::Edit;

        // Editor resources
        ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;
    };
}

#endif /* !FOX_EDITOR_STATE_HPP_ */
