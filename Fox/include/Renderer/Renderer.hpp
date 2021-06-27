//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_RENDERER_HPP
#define FOXENGINE_RENDERER_HPP

//#include <glad/glad.h>
//#include "DebugBreak.hpp"
//
//#define ASSERT(x) if (!(x)) debug_break();
//#define GLCall(x) GLClearError();\
//    x;\
//    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
//
//void GLClearError();
//bool GLLogCall(const char *function, const char *file, int line);
//
//
//namespace fox
//{
//    class VertexArray;
//    class IndexBuffer;
//    class Shader;
//
//    class Renderer
//    {
//    public:
//        void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
//        void Clear() const;
//    private:
//
//    };
//}

#include "Plugin/IPlugin.hpp"
#include "OrthographicCamera.hpp"

namespace fox
{
    class Renderer
    {
    public:
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
