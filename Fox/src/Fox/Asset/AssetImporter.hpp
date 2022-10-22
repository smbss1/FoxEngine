//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETIMPORTER_HPP
#define FOXENGINE_ASSETIMPORTER_HPP

#include "AssetSerializer.hpp"

namespace fox
{
    class AssetImporter
    {
    public:
        static void Init();
        static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);
        static void Serialize(const Ref<Asset>& asset);
        static bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset);

    private:
        static std::unordered_map<AssetType, scope<AssetSerializer>> s_Serializers;
    };
}

#endif //FOXENGINE_ASSETIMPORTER_HPP
