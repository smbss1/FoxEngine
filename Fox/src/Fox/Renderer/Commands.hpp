//
// Created by samuel on 06/11/22.
//

#ifndef FOX_LEXER_COMMANDS_HPP
#define FOX_LEXER_COMMANDS_HPP

#include <memory>
#include <functional>
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "Core/Ref.hpp"
#include "Renderer2D.hpp"
#include "Core/Logger/Logger.hpp"
#include "Renderer/Camera.hpp"

namespace fox
{
    class Entity;
    class RenderPass;
    class Scene;
    class UniformBuffer;

    struct ClearColorCmd
    {
        WeakRef<RenderPass> m_Pass;
        ClearColorCmd(WeakRef<RenderPass> renderPass) : m_Pass(renderPass) {}

        void Execute();
    };

    struct ClearAttachmentCmd
    {
        WeakRef<RenderPass> m_Pass;
        uint32_t m_AttachmentIndex;
        int m_Value;

        ClearAttachmentCmd(WeakRef<RenderPass> renderPass, uint32_t attachmentIndex, int value)
            : m_Pass(renderPass)
            , m_AttachmentIndex(attachmentIndex)
            , m_Value(value) {}

        void Execute();
    };

    struct PushCameraUniformCmd
    {
        WeakRef<UniformBuffer> m_CameraUniformBuffer;
        Renderer2D::CameraData m_CameraBuffer;

        explicit PushCameraUniformCmd(WeakRef<UniformBuffer> CameraUniformBuffer, const Renderer2D::CameraData& data)
            : m_CameraUniformBuffer(CameraUniformBuffer)
        {
            m_CameraBuffer = data;
        }

        void Execute();
    };

    struct FunctionCmd
    {
        std::function<void()> func;

        explicit FunctionCmd(std::function<void()>&& f) : func(std::move(f)) { }

        void Execute()
        {
            func();
        }
    };

    struct CheckHoverEntity
    {
        glm::vec2 m_ViewportSize;
        glm::vec2 m_MousePos;

        Entity& m_HoverEntity;
        WeakRef<RenderPass> m_RenderPass;
        WeakRef<Scene> m_ActiveScene;

        void Execute();
    };
}


#endif //FOX_LEXER_COMMANDS_HPP
