//
// Created by samuel on 26/06/2021.
//

#include <Plugin/IPlugin.hpp>
#include <Renderer/RendererCommand.hpp>
#include "Renderer/Renderer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexArray.hpp"

namespace fox
{
    Renderer::SceneData Renderer::m_SceneData = Renderer::SceneData();

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        m_SceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const ref<Shader>& shader, const ref<VertexArray> &pVertexArray, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetUniform("u_ViewProjection", m_SceneData.ViewProjectionMatrix);
        shader->SetUniform("u_Transform", transform);

        pVertexArray->Bind();
        RendererCommand::DrawIndexed(pVertexArray);
    }
}