//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_IPLUGIN_HPP
#define ECS_IPLUGIN_HPP

#include <string>
#include "common.hpp"

class World;

namespace fox
{
    class Texture2D;
    class Framebuffer;
    class Camera;
    class EditorCamera;
    class Application;
    class IndexBuffer;
    class VertexBuffer;
    class VertexArray;
    class RendererAPI;
    class Shader;

    struct FramebufferSpecification;

    class GraphicsContext;

    class Window;
    class WindowProps;

    class IPlugin
    {
    public:
        virtual void plug(Application &app) = 0;
        virtual void unplug(Application &app) = 0;

        virtual const std::string &get_name() const = 0;

        virtual int get_version() const = 0;
    };

    class WindowPlugin : public IPlugin
    {
    public:
        virtual ref<Window> CreateWindow(const WindowProps& props) = 0;
    };

    class GraphicPlugin : public IPlugin
    {
    public:
        virtual void update() = 0;
        virtual scope<GraphicsContext> CreateGraphicsContext(void* window) = 0;
        virtual ref<IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t count) = 0;
        virtual ref<VertexBuffer> CreateVertexBuffer(uint32_t size) = 0;
        virtual ref<VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size) = 0;
        virtual ref<VertexArray> CreateVertexArray() = 0;
        virtual ref<RendererAPI> CreateRenderer() = 0;
        virtual ref<Shader> CreateShader(const std::string &path) = 0;
        virtual ref<Shader> CreateShader(const std::string &name, const std::string &vertexSrc, const std::string &fragSrc) = 0;

        virtual ref<Texture2D> create_texture(uint32_t width, uint32_t height) = 0;
        virtual ref<Texture2D> create_texture(const std::string& path) = 0;
        virtual ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) = 0;
        virtual ref<Camera> create_camera() = 0;
    };
}

#define PLUGIN_API(name, version, function_import)                        \
    extern "C" void LibImport(fox::Application& app)                           \
    {                                                                     \
        function_import(app);                                          \
    }                                                                     \

#endif //ECS_IPLUGIN_HPP
