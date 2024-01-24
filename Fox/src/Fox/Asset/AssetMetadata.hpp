//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETMETADATA_HPP
#define FOXENGINE_ASSETMETADATA_HPP

#include "Asset.hpp"

#include "Core/Base.hpp"

namespace fox
{
    struct AssetMetadata
    {
        AssetHandle Handle = 0;
        AssetType Type;

        fs::path FilePath;
        bool IsDataLoaded = false;
        bool IsMemoryAsset = false;

        bool IsValid() const { return Handle != 0 && !IsMemoryAsset; }
    };
}

#endif //FOXENGINE_ASSETMETADATA_HPP
