//
// Created by samuel on 27/06/2021.
//

#include <Core/Assert.hpp>
#include "RendererCommand.hpp"
#include "RendererAPI.hpp"

namespace fox
{
    ref<RendererAPI> RendererCommand::m_spRenderer = RendererAPI::Create();

    void RendererCommand::Init()
    {
        m_spRenderer->Init();
    }

    void RendererCommand::Shutdown()
    {
        m_spRenderer = nullptr;
    }

    void RendererCommand::SetClearColor(const glm::vec4 &color)
    {
        m_spRenderer->SetClearColor(color);
    }

    void RendererCommand::Clear()
    {
        m_spRenderer->Clear();
    }

    void RendererCommand::DrawIndexed(const fox::ref<fox::VertexArray> &pVertexArray, uint32_t uIndexCount)
    {
        m_spRenderer->DrawIndexed(pVertexArray, uIndexCount);
    }

    void RendererCommand::SetRendererAPI(const ref<RendererAPI> &pRendererAPI)
    {
        m_spRenderer = pRendererAPI;
    }

    void RendererCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_spRenderer->SetViewport(x, y, width, height);
    }
}