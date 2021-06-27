//
// Created by samuel on 27/06/2021.
//

#include <Core/Assert.hpp>
#include "Renderer/RendererCommand.hpp"
#include "Renderer/RendererAPI.hpp"

namespace fox
{
    ref<RendererAPI> RendererCommand::m_spRenderer = nullptr;

    void RendererCommand::SetClearColor(const glm::vec4 &color)
    {
        m_spRenderer->SetClearColor(color);
    }

    void RendererCommand::Clear()
    {
        m_spRenderer->Clear();
    }

    void RendererCommand::DrawIndexed(const fox::ref<fox::VertexArray> &pVertexArray)
    {
        m_spRenderer->DrawIndexed(pVertexArray);
    }

    void RendererCommand::SetRendererAPI(const ref<RendererAPI> &pRendererAPI)
    {
        m_spRenderer = pRendererAPI;
    }
}