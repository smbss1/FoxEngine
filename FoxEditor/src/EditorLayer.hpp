/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_STATE_HPP_
#define FOX_EDITOR_STATE_HPP_

#include "Utils/Vec2.hpp"

#include "FoxEngine.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Renderer/EditorCamera.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Events/KeyEvent.hpp"

#include "Core/Layer.hpp"
#include "Panels/Panel.hpp"
#include "Core/UserPreferences.hpp"
#include "Renderer/RenderPass.hpp"
#include "SignalDefs.hpp"

namespace fox
{
    class PanelManager;
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(Ref<UserPreferences> UserPrefs);
        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
        void OnEvent(Event& event) override;
        void OpenScene(const std::filesystem::path& path);

        static EditorLayer& Get() { return *s_Instance; }

    private:

        void BeginDockspace();
        void EndDockspace();
        void UI_MenuBar();

        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

         void NewProject();
        void OpenProject();
        void OpenProject(const std::string& filepath);
        void CloseProject(bool unloadProject = true);
        void SaveProject();
        void EmptyProject();

        void NewScene();
        void OpenScene();
        void SaveScene();
        void SaveSceneAs();

        void NewScript();

        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();

        void OnDuplicateEntity();

        // UI Panel
        void UI_Toolbar();
        void OnOverlayRender();

    private:
        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        bool m_bViewportFocused = false;
        bool m_bViewportHovered = false;
        Ref<Scene> m_pActiveScene;
        Ref<Scene> m_pEditorScene;
        scope<PanelManager> m_PanelManager;

        Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
        EditorCamera m_oEditorCamera;

        int m_iGizmoType = -1;

        glm::vec2 m_vViewportBounds[2];
        Entity m_oHoveredEntity;
        std::string m_EditorScenePath;

        fox::Ref<fox::UserPreferences> m_UserPrefs;
        enum class SceneState
        {
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_SceneState = SceneState::Edit;

        fox::Ref<RenderPass> m_FinalRenderPass;

        // Events
        OnImGuiRenderEvent m_OnImGuiRenderEvent;
        OnOverlayRenderEvent m_OnOverlayRenderEvent;

        static EditorLayer* s_Instance;
    };
}

#endif /* !FOX_EDITOR_STATE_HPP_ */
