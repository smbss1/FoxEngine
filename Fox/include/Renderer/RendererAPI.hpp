//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_RENDERERAPI_HPP
#define FOXENGINE_RENDERERAPI_HPP

#include "glm/glm.hpp"
#include "Plugin/IPlugin.hpp"

namespace fox
{
    class VertexArray;
    class RendererAPI
    {
    public:
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawIndexed(const ref<VertexArray>& pVertexArray) = 0;

        static GraphicPlugin* GetGraphicPlugin();
        static void SetGraphicPlugin(GraphicPlugin* pGraphic);

    private:
        static GraphicPlugin* s_Graphic;
    };
}

#endif //FOXENGINE_RENDERERAPI_HPP
