////
//// Created by samuel on 26/06/2021.
////
//
//#include <string>
//#include <iostream>
#include <Plugin/IPlugin.hpp>
#include <Renderer/RendererCommand.hpp>
#include "Renderer/Renderer.hpp"
//#include "VertexArray.hpp"
//#include "VertexBuffer.hpp"
//#include "IndexBuffer.hpp"
//#include "Shader.hpp"
//
//void GLClearError()
//{
//    while (glGetError() != GL_NO_ERROR);
//}
//
//bool GLLogCall(const char *function, const char *file, int line)
//{
//    bool res = true;
//    GLenum errorCode;
//    while ((errorCode = glGetError()) != GL_NO_ERROR) {
//        std::string error;
//        switch (errorCode) {
//            case GL_INVALID_ENUM:
//                error = "INVALID_ENUM";
//                break;
//            case GL_INVALID_VALUE:
//                error = "INVALID_VALUE";
//                break;
//            case GL_INVALID_OPERATION:
//                error = "INVALID_OPERATION";
//                break;
//            case GL_STACK_OVERFLOW:
//                error = "STACK_OVERFLOW";
//                break;
//            case GL_STACK_UNDERFLOW:
//                error = "STACK_UNDERFLOW";
//                break;
//            case GL_OUT_OF_MEMORY:
//                error = "OUT_OF_MEMORY";
//                break;
//            case GL_INVALID_FRAMEBUFFER_OPERATION:
//                error = "INVALID_FRAMEBUFFER_OPERATION";
//                break;
//        }
//        std::cerr << "[OpenGL] " << error << " | " << function << " " << file << " (" << line << ")" << std::endl;
//        res = false;
//    }
//    return res;
//}
//
//namespace fox
//{
//    void Renderer::Draw(const fox::VertexArray &va, const fox::IndexBuffer &ib, const fox::Shader &shader)
//    {
//        shader.Bind();
//
//        va.Bind();
//        ib.Bind();
//        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
//    }
//
//    void Renderer::Clear() const
//    {
////        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
//        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
//    }
//}

namespace fox
{
    void Renderer::BeginScene()
    {
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const ref<VertexArray> &pVertexArray)
    {
        RendererCommand::DrawIndexed(pVertexArray);
    }
}