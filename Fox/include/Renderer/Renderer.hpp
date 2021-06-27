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

namespace fox
{
    class Renderer
    {
    public:
        static void BeginScene();
        static void EndScene();
        static void Submit(const ref<VertexArray>& pVertexArray);
    };
}

#endif //FOXENGINE_RENDERER_HPP
