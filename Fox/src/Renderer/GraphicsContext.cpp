//
// Created by samuel on 27/06/2021.
//

#include <Renderer/RendererAPI.hpp>
#include "Renderer/GraphicsContext.hpp"

namespace fox
{
    scope<GraphicsContext> GraphicsContext::Create(void* window)
    {
        return RendererAPI::GetGraphicPlugin()->CreateGraphicsContext(window);
    }
}