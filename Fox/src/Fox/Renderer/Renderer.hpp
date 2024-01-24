//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_RENDERER_HPP
#define FOXENGINE_RENDERER_HPP

#include "glm/glm.hpp"
#include "RenderPass.hpp"

namespace fox
{
    class RenderCommandQueue;
    class RendererAPI;
    class VertexArray;
    class Material;
    class Mesh;

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass(Ref<RenderPass> renderPass);
        static void EndRenderPass();

        static void OnWindowResize(unsigned int width, unsigned int height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void DrawIndexed(const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount = 0);
        static void RenderGeometry(Ref<Material> shader, const Ref<VertexArray>& pVertexArray, uint32_t uIndexCount = 0);
        static void RenderInstanceGeometry(Ref<Material> material, const Ref<Mesh>& mesh, uint32_t instanceCount);

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount);
        static void SetLineWidth(float width);

        static void WaitAndRender();

    private:
        static scope<RendererAPI> m_spRenderer;
    };
}

#endif //FOXENGINE_RENDERER_HPP
