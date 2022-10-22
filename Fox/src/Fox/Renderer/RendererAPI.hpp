//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_RENDERERAPI_HPP
#define FOXENGINE_RENDERERAPI_HPP

#include "glm/glm.hpp"
#include "common.hpp"

namespace fox
{
    class VertexArray;
    class RendererAPI : public RefCounted
    {
    public:
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawIndexed(const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount = 0) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
        virtual void SetLineWidth(float width) = 0;

        static scope<RendererAPI> Create();

    private:
        // static GraphicPlugin* s_Graphic;
    };
}

#endif //FOXENGINE_RENDERERAPI_HPP
