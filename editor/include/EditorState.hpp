/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_STATE_HPP_
#define FOX_EDITOR_STATE_HPP_

#include <Math/Vec2.hpp>
#include <json.hpp>
#include <FileSystem/FileWatcher.hpp>

#include "FoxEngine.hpp"
#include "SceneHierarchyPanel.hpp"
#include "Renderer/EditorCamera.hpp"
#include "ContentBrowserPanel.hpp"
#include "AnimationEditor.hpp"

namespace fox
{
    class EditorState : public State
    {
    public:
        EditorState()
                : m_CameraController(1280.f / 720.f), State("Editor")
        {}

        ~EditorState() override;

        void OnEnter() override;

        void OnExit() override;

        void OnEvent(fox::Event &event) override;

        void OnUpdate() override;

        void OnImGui() override;

    private:

        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void NewProject();
        void OpenProject();

        void NewScene();
        void OpenScene();
        void SaveScene();
        void SaveSceneAs();

        void NewScript();

        /**
         * @brief On runtime start, this function will be called from EventSystem
         */
        void OnRuntimeStart();

        /**
         * @brief On runtime stop, this function will be called from EventSystem
         */
        void OnRuntimeStop();

        /**
         * @brief Start init the file watcher for hot reload asset
         */
        void InitFileWatcher();

    private:
        bool m_bIsRunning = false;

        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        fox::ref<Framebuffer> m_Framebuffer;
        bool m_bViewportFocused = false;
        bool m_bViewportHovered = false;
        ref<Scene> m_pActiveScene;
        ContentBrowserPanel m_ContentBrowserPanel;
        SceneHierarchyPanel m_SceneHierarchyPanel;

        EditorCamera m_oEditorCamera;

        int m_iGizmoType = -1;

        glm::vec2 m_vViewportBounds[2];

        Entity m_oHoveredEntity;

        json::Value m_oEditorConfig;

        scope<FileWatcher> m_oWatcher;

        fox::AnimationEditor m_AnimationEditor;

        //--
        //      DELEGATES
        //--
        event::Delegate<void()> m_OnRuntimeStart;
        event::Delegate<void()> m_OnRuntimeStop;

    };
}

#endif /* !FOX_EDITOR_STATE_HPP_ */
