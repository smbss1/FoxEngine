//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_RENDERERCOMMAND_HPP
#define FOXENGINE_RENDERERCOMMAND_HPP

#include <glm/vec4.hpp>
#include "common.hpp"

namespace fox
{
    class RendererAPI;
    class VertexArray;

    class RendererCommand
    {
    public:
        static void Init();
        static void Shutdown();
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void DrawIndexed(const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount = 0);

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount);
        static void SetLineWidth(float width);

    private:
        static scope<RendererAPI> m_spRenderer;
    };
}

#endif //FOXENGINE_RENDERERCOMMAND_HPP
