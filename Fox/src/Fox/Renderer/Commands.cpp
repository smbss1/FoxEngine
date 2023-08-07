//
// Created by samuel on 06/11/22.
//

#include "Commands.hpp"
#include "Ecs/Entity.hpp"
#include "RenderPass.hpp"
#include "Renderer.hpp"
#include "UniformBuffer.hpp"

namespace fox
{
    void ClearColorCmd::Execute()
    {
        if (!m_Pass.IsValid())
            return;

        Ref<Framebuffer> framebuffer = m_Pass->GetSpecs().RenderTarget;

        framebuffer->Bind();
        Renderer::SetClearColor(framebuffer->GetSpecification().ClearColor);
        Renderer::Clear();
    }

    void ClearAttachmentCmd::Execute()
    {
        if (!m_Pass.IsValid())
            return;

        Ref<Framebuffer> framebuffer = m_Pass->GetSpecs().RenderTarget;

        // Clear our entity ID attachment to -1
        framebuffer->ClearAttachment(1, -1);
    }

    void PushCameraUniformCmd::Execute()
    {
        if (!m_CameraUniformBuffer.IsValid())
            return;

        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(Renderer2D::CameraData));
    }

    void CheckHoverEntity::Execute()
    {
        if (!m_RenderPass.IsValid() || !m_ActiveScene.IsValid())
            return;

        // Check the position of the mouse is in the viewport boundary
        if (m_MousePos.x >= 0 && m_MousePos.y >= 0 && m_MousePos.x < m_ViewportSize.x && m_MousePos.y < m_ViewportSize.y)
        {
            int pixelData = m_RenderPass->GetSpecs().RenderTarget->ReadPixel(1, m_MousePos.x, m_MousePos.y);
            m_HoverEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.Raw());
        }
    }
}
