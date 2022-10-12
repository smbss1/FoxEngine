

#ifndef FOX_IGRAPHIC_HPP_
#define FOX_IGRAPHIC_HPP_

#include <vector>
#include <string>

#include "Core/Application.hpp"
#include "IPlugin.hpp"

namespace fox
{
//    class Input;
//    class IGraphic
//    {
//    public:
//        virtual ~IGraphic() = 0;
//        virtual void init(Application&) = 0;
//        virtual void shutdown(Application&) = 0;
//        virtual void poll_event(Input&) = 0;
//        virtual void draw(Application&) = 0;
//        virtual void set_vsync(bool value) = 0;
//        virtual bool is_vsync() = 0;
//        virtual bool quit_requested() = 0;
//    };

//    // ----------------------------------------------------------------------- //
//
//    class Renderer {}; // Dummy
//
//    // ----------------------------------------------------------------------- //
//
    /// <summary>Manages graphics related stuff for the engine</summary>
    class GraphicsService
    {
        /// <summary>Graphics driver interface for the engine</summary>
    public:
//        class GraphicsDriver
//        {
//        public:
//            /// <summary>Releases resources of a driver after use</summary>
//            virtual ~GraphicsDriver() = default;
//            /// <summary>Gets the name of the graphics driver</summary>
//            virtual const std::string &getName() const = 0;
//            /// <summary>Creates a renderer using the driver's rendering API</summary>
//            virtual std::unique_ptr<Renderer> createRenderer() = 0;
//        };

        /// <summary>A vector of graphics drivers</summary>
    private:
        typedef std::vector<ref<fox::GraphicPlugin>> GraphicsPluginVector;

        /// <summary>Releases the resources of the graphics server</summary>
    public:
        FOX_API ~GraphicsService() = default;

        /// <summary>Allows plugins to add new graphics drivers</summary>
        /// <param name="graphicsDriver">Graphics driver that will be added</param>
        FOX_API void AddGraphicsPlugin(ref<fox::GraphicPlugin> pGraphicPlugin)
        {
            m_vGraphicsPlugin.push_back(pGraphicPlugin);
        }

        /// <summary>Gets the total number of registered graphics drivers</summary>
        FOX_API size_t GetCount() const
        {
            return m_vGraphicsPlugin.size();
        }

        /// <summary>Accesses a driver by its index</summary>
        FOX_API fox::GraphicPlugin& GetPlugin(size_t idx)
        {
            return *m_vGraphicsPlugin.at(idx);
        }

    private:
        /// <summary>All available graphics drivers</summary>
        GraphicsPluginVector m_vGraphicsPlugin;

    };

} // namespace fox

#endif