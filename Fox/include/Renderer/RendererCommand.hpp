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
        static void DrawIndexed(const ref<VertexArray>& pVertexArray);

        static void SetRendererAPI(const ref<RendererAPI>& pRendererAPI);
    private:
        static ref<RendererAPI> m_spRenderer;
    };
}

#endif //FOXENGINE_RENDERERCOMMAND_HPP
