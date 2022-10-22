//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_OPENGLRENDERERAPI_HPP
#define FOXENGINE_OPENGLRENDERERAPI_HPP

#include "Renderer/RendererAPI.hpp"
#include <glad/glad.h>

namespace fox
{
    class OpenGLRendererAPI : public RendererAPI
    {
        void Init() override;
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;
        void DrawIndexed(const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount = 0) override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
        virtual void SetLineWidth(float width) override;
    };
}

#endif //FOXENGINE_OPENGLRENDERERAPI_HPP
