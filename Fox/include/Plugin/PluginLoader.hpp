//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_PLUGINLOADER_HPP
#define ECS_PLUGINLOADER_HPP

#include "SharedLib.hpp"
#include "Core/Core.hpp"

namespace fox
{
    class PluginManager;
    class PluginLoader
    {
    private:
        /// <summary>Signature for the version query function</summary>
//        typedef int GetEngineVersionFunction();
        /// <summary>Signature for the plugin's registration function</summary>
        typedef void RegisterPluginFunction(PluginManager &);

        /// <summary>Initializes and loads a plugin</summary>
        /// <param name="filename">Filename of the plugin to load</summary>
    public:
        FOX_API PluginLoader(const std::string &filename);
        /// <summary>Copies an existing plugin instance</summary>
        FOX_API PluginLoader(const PluginLoader &other);
        /// <summary>Unloads the plugin</summary>
        FOX_API ~PluginLoader();

        /// <summary>Queries the plugin for its expected engine version</summary>
    public:
//        FOX_API int GetVersion() const
//        {
//            return m_pGetVersionAddress();
//        }

        /// <summary>Register the plugin to a kernel</summary>
        /// <param name="kernel">Kernel the plugin should register to</param>
        FOX_API void RegisterPlugin(PluginManager &plugin_manager)
        {
            this->registerPluginAddress(plugin_manager);
        }

    private:
        /// <summary>Function to query for the expected engine version</summary>
//        GetEngineVersionFunction *m_pGetVersionAddress;
        /// <summary>Registers the plugin with the kernel</summary>
        RegisterPluginFunction *registerPluginAddress;

        SharedLib loader;
    };
}

#endif //ECS_PLUGINLOADER_HPP
