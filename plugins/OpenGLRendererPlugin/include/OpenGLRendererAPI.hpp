//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_OPENGLRENDERERAPI_HPP
#define FOXENGINE_OPENGLRENDERERAPI_HPP

#include "Renderer/RendererAPI.hpp"
#include <glad/glad.h>
#include "DebugBreak.hpp"

#define ASSERT(x) if (!(x)) debug_break();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

namespace fox
{
    class OpenGLRendererAPI : public RendererAPI
    {
        void Init() override;
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;
        void DrawIndexed(const ref<VertexArray>& pVertexArray) override;
    };
}

#endif //FOXENGINE_OPENGLRENDERERAPI_HPP
