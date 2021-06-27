//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_RENDERER_HPP
#define FOXENGINE_RENDERER_HPP

#include "Plugin/IPlugin.hpp"
#include "OrthographicCamera.hpp"

namespace fox
{
    class Renderer
    {
    public:
        static void Init();
        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();
        static void Submit(const ref<Shader>& shader, const ref<VertexArray>& pVertexArray, const glm::mat4& transform = glm::mat4(1.0f));

    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData m_SceneData;
    };
}

#endif //FOXENGINE_RENDERER_HPP
