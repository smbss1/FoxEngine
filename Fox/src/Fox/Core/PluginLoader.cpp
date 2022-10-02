//
// Created by samuel on 22/06/2021.
//

#include "Core/PluginLoader.hpp"

namespace fox
{
    PluginLoader::PluginLoader(const std::string &filepath) :
//            getEngineVersionAddress(0),
            registerPluginAddress(0) {

        try {
            // Try to load the plugin as a shared library
            loader.open(filepath);

            // Locate the plugin's exported functions
            loader.sym("RegisterPlugin", registerPluginAddress);
        }
        catch(std::runtime_error & e) {
            loader.close();
            throw e;
        }
    }

    /// <summary>
    ///   Creates a copy of a plugin that has already been loaded.
    ///   Required to provide correct semantics for storing plugins in
    ///   an STL map container.
    /// </summary>
    /// <param name="other">Other plugin instance to copy</param>
    PluginLoader::PluginLoader(const PluginLoader &other) :
//            m_pGetVersionAddress(other.getEngineVersionAddress),
            registerPluginAddress(other.registerPluginAddress) {

        // Increase DLL reference counter
    }

    /// <summary>
    ///   Destroys the plugin, unloading its library when no more references
    ///   to it exist.
    /// </summary>
    PluginLoader::~PluginLoader()
    {
        loader.close();
    }
}