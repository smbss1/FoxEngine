//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_PLUGINMANAGER_HPP
#define ECS_PLUGINMANAGER_HPP

#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include "Utils/Path.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Core.hpp"
#include "Plugin/IGraphic.hpp"
#include "PluginLoader.hpp"
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
                FOX_CORE_INFO("Load Plugin: %", path.string());
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
            FOX_CORE_INFO("Initialize plugins");
            for (auto& it : m_vPlugins)
            {
                it->plug(app);
            }
            FOX_CORE_INFO("Initialization of plugins done");
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
