//
// Created by samuel on 27/06/2021.
//

#include "Renderer/RendererAPI.hpp"

namespace fox
{
    GraphicPlugin* RendererAPI::s_Graphic = nullptr;

    GraphicPlugin* RendererAPI::GetGraphicPlugin()
    {
        return s_Graphic;
    }

    void RendererAPI::SetGraphicPlugin(GraphicPlugin *pGraphic)
    {
        s_Graphic = pGraphic;
    }
}