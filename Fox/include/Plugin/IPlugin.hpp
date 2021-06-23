//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_IPLUGIN_HPP
#define ECS_IPLUGIN_HPP

class World;

namespace fox
{
    class IPlugin
    {
    public:
        virtual void plug(Application &app) = 0;

        virtual const std::string &get_name() const = 0;

        virtual int get_version() const = 0;
    };

    class GraphicPlugin : public IPlugin
    {
    public:
        virtual void poll_event() = 0;
        virtual void draw() = 0;
    };
}

#define PLUGIN_API(name, version, function_import)                        \
    extern "C" void LibImport(fox::Application& app)                           \
    {                                                                     \
        function_import(app);                                          \
    }                                                                     \

#endif //ECS_IPLUGIN_HPP
