/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Sandbox
*/

#ifndef FOX_EDITOR_STATE_HPP_
#define FOX_EDITOR_STATE_HPP_

#include <Utils/Vec2.hpp>
#include "FoxEngine.hpp"
#include "SceneHierarchyPanel.hpp"
#include "Renderer/EditorCamera.hpp"

namespace fox
{
    class EditorState : public State
    {
    public:
        EditorState()
                : m_CameraController(1280.f / 720.f), State("Editor")
        {}

        ~EditorState() override = default;

        void OnEnter() override;

        void OnExit() override;

        void OnEvent(fox::Event &event) override;

        void OnUpdate() override;

        void OnImGui() override;

    private:

        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void NewScene();

        void OpenScene();

        void SaveSceneAs();

    private:
        fox::OrthographicCameraController m_CameraController;

        Vec2 m_oViewportSize = {0, 0};
        fox::ref<Framebuffer> m_Framebuffer;
        bool m_bViewportFocused = false;
        bool m_bViewportHovered = false;
        SceneHierarchyPanel m_SceneHierarchyPanel;
        ref<Scene> m_pActiveScene;

        EditorCamera m_oEditorCamera;

        int m_iGizmoType = -1;

        glm::vec2 m_vViewportBounds[2];

        Entity m_oHoveredEntity;
    };
}

#endif /* !FOX_EDITOR_STATE_HPP_ */
