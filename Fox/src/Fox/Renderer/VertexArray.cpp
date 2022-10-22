//
// Created by samuel on 26/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "common.hpp"
#include "Renderer/VertexArray.hpp"
#include "OpenGL/OpenGLVertexArray.hpp"

namespace fox
{
    Ref<VertexArray> VertexArray::Create()
    {
        return new_ref<OpenGLVertexArray>();
    }
}
