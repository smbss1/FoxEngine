//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_IPLUGIN_HPP
#define ECS_IPLUGIN_HPP

class World;

namespace fox
{
    class Texture2D;
    class Framebuffer;
    class Camera;
    class EditorCamera;

    class IPlugin
    {
    public:
        virtual void plug(Application &app) = 0;
        virtual void unplug(Application &app) = 0;

        virtual const std::string &get_name() const = 0;

        virtual int get_version() const = 0;
    };

    class GraphicPlugin : public IPlugin
    {
    public:
        virtual void poll_event() = 0;
        virtual void draw() = 0;
        virtual ref<Texture2D> create_texture(uint32_t width, uint32_t height) = 0;
        virtual ref<Texture2D> create_texture(const std::string& path) = 0;
        virtual ref<Framebuffer> create_frame_buffer(uint32_t width, uint32_t height) = 0;
        virtual ref<Camera> create_camera() = 0;
        virtual ref<EditorCamera> create_editor_camera() = 0;
    };
}

#define PLUGIN_API(name, version, function_import)                        \
    extern "C" void LibImport(fox::Application& app)                           \
    {                                                                     \
        function_import(app);                                          \
    }                                                                     \

#endif //ECS_IPLUGIN_HPP
