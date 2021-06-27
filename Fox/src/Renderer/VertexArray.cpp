//
// Created by samuel on 26/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "common.hpp"
#include "Renderer/VertexArray.hpp"

namespace fox
{
    ref<VertexArray> VertexArray::Create()
    {
        return RendererAPI::GetGraphicPlugin()->CreateVertexArray();
    }
}