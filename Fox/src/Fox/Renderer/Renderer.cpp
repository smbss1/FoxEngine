//
// Created by samuel on 26/06/2021.
//

#include "Renderer/RendererCommand.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/RendererAPI.hpp"
#include "Material.hpp"
#include "Commands.hpp"
#include "Mesh.hpp"

namespace fox
{
    scope<RendererAPI> Renderer::m_spRenderer = RendererAPI::Create();

    static Ref<RenderPass> s_CurrentRenderPass = nullptr;

    void Renderer::Init()
    {
        m_spRenderer->Init();

        RendererCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();
        RendererCommand::Shutdown();
        m_spRenderer = nullptr;
    }

    void Renderer::SetClearColor(const glm::vec4 &color)
    {
        m_spRenderer->SetClearColor(color);
    }

    void Renderer::Clear()
    {
        m_spRenderer->Clear();
    }

    void Renderer::RenderGeometry(Ref<Material> material, const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount)
    {
        WeakRef<Material> weakMat = material;
        RendererCommand::Push<FunctionCmd>([weakMat, pVertexArray, uIndexCount]() mutable {
            if (weakMat.IsValid())
            {
                weakMat->UpdateForRendering();
                m_spRenderer->DrawIndexed(pVertexArray, uIndexCount);
                weakMat->EndRendering();
            }
        });
    }

    void Renderer::RenderInstanceGeometry(Ref<Material> material, const Ref<Mesh>& mesh, uint32_t instanceCount)
    {
        WeakRef<Material> weakMat = material;
        RendererCommand::Push<FunctionCmd>([weakMat, mesh, instanceCount]() mutable {
            if (weakMat.IsValid())
            {
                weakMat->UpdateForRendering();
                m_spRenderer->DrawElementsInstanced(mesh->VertexArray, mesh->VertexArray->GetIndexBuffer()->GetCount(), instanceCount);
                weakMat->EndRendering();
            }
            });
    }

    void Renderer::DrawIndexed(const fox::Ref<fox::VertexArray> &pVertexArray, uint32_t uIndexCount)
    {
        m_spRenderer->DrawIndexed(pVertexArray, uIndexCount);
    }

    void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_spRenderer->SetViewport(x, y, width, height);
    }

    void Renderer::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        m_spRenderer->DrawLines(vertexArray, vertexCount);
    }

    void Renderer::SetLineWidth(float width)
    {
        m_spRenderer->SetLineWidth(width);
    }

    void Renderer::OnWindowResize(unsigned int width, unsigned int height)
    {
        m_spRenderer->SetViewport(0, 0, width, height);
    }

    void Renderer::BeginRenderPass(Ref<RenderPass> renderPass)
    {
        s_CurrentRenderPass = renderPass;
        RendererCommand::Push<ClearColorCmd>(renderPass);
        RendererCommand::Push<ClearAttachmentCmd>(renderPass, 1, -1);

//        RendererCommand::Push<FunctionCmd>([=] mutable {
//            s_CurrentRenderPass->GetSpecs().RenderTarget->Bind();
//            SetClearColor(s_CurrentRenderPass->GetSpecs().RenderTarget->GetSpecification().ClearColor);
//            Clear();
//
//            // Clear our entity ID attachment to -1
//            s_CurrentRenderPass->GetSpecs().RenderTarget->ClearAttachment(1, -1);
//        });
    }

    void Renderer::EndRenderPass()
    {
        RendererCommand::Push<FunctionCmd>([=] {
            s_CurrentRenderPass->GetSpecs().RenderTarget->Unbind();
        });
    }

    void Renderer::WaitAndRender()
    {
//        FOX_PROFILE_FUNC();
        RendererCommand::WaitAndRender();
    }
}
