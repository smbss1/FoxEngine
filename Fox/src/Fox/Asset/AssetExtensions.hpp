//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETEXTENSIONS_HPP
#define FOXENGINE_ASSETEXTENSIONS_HPP

#include <unordered_map>

#include "AssetTypes.hpp"

namespace fox
{
    inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
        {
            // Hazel types
            { ".foxscene", AssetType::Scene },
            { ".foxprefab", AssetType::Prefab },
//            { ".hsoundc", AssetType::SoundConfig },
//            { ".hpm", AssetType::PhysicsMat },

            // Textures
            { ".png", AssetType::Texture },
            { ".jpg", AssetType::Texture },
            { ".jpeg", AssetType::Texture },

            // Audio
            { ".wav", AssetType::Audio },
            { ".ogg", AssetType::Audio },

            // Fonts
            { ".ttf", AssetType::Font },
            { ".ttc", AssetType::Font },
            { ".otf", AssetType::Font },

            // Graphs
//            { ".sound_graph", AssetType::SoundGraphSound }
        };

}

#endif //FOXENGINE_ASSETEXTENSIONS_HPP
