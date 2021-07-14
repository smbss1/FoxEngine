//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_PLUGINMANAGER_HPP
#define ECS_PLUGINMANAGER_HPP

#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <FileSystem/Path.hpp>
#include <Logger/Logger.hpp>
#include "Core/Core.hpp"
#include "Plugin/IGraphic.hpp"
#include "Plugin/PluginLoader.hpp"
#include "Plugin/IPlugin.hpp"

namespace fox
{
    class PluginManager
    {
        /// Map of plugins by their associated file names
        using PluginMap = std::map<std::string, ref<fox::PluginLoader>>;
        using PluginVec = std::vector<ref<fox::IPlugin>>;
//        using GraphicsPlugins = std::vector<fox::GraphicPlugin*>;

    public:

        FOX_API void FindAndLoadPlugins(Path oPath)
        {
            Path::paths vPluginsPaths = oPath.get_all_files(DL_EXT);
            for (auto path : vPluginsPaths)
            {
                fox::info("Load Plugin: %", path.string());
                LoadPlugin(path.string());
            }
        }

        /// Loads a plugin
        /// <param name="filename">File the plugin will be loaded from</param>
        FOX_API void LoadPlugin(const std::string &filename)
        {
            if (this->loadedPlugins.find(filename) == this->loadedPlugins.end()) {
                this->loadedPlugins.insert(
                        PluginMap::value_type(filename, new_ref<fox::PluginLoader>(filename))
                ).first->second->RegisterPlugin(*this);
            }
        }

        FOX_API void AddPlugin(const ref<IPlugin>& plugin)
        {
            auto it = std::find(m_vPlugins.begin(), m_vPlugins.end(), plugin);
            if (it == m_vPlugins.end())
                m_vPlugins.push_back(plugin);
        }

        GraphicsService& GetGraphics()
        {
            return m_oGraphicService;
        }

        ref<fox::WindowPlugin> GetWindowPlugin()
        {
            return m_pWindowPlugin;
        }

        void SetWindowPlugin(const ref<fox::WindowPlugin>& pPlugin)
        {
            m_pWindowPlugin = pPlugin;
        }

        void InitializePlugins(fox::Application& app)
        {
            fox::info("Initialize plugins");
            for (auto& it : m_vPlugins)
            {
                it->plug(app);
            }
            fox::info("Initialization of plugins done");
        }

    private:
        /// All plugins currently loaded
        PluginMap loadedPlugins;
        PluginVec m_vPlugins;

        /// Manages graphics-related tasks for the engine
        GraphicsService m_oGraphicService;

        ref<fox::WindowPlugin> m_pWindowPlugin;
    };
}

#endif //ECS_PLUGINMANAGER_HPP
