//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETIMPORTER_HPP
#define FOXENGINE_ASSETIMPORTER_HPP

#include "AssetMetadata.hpp"

namespace fox
{
    class AssetImporter
    {
    public:
        static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);
        static void Serialize(const Ref<Asset>& asset);

        static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
    };
}

#endif //FOXENGINE_ASSETIMPORTER_HPP
