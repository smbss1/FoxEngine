//
// Created by samuel on 22/10/22.
//

#include "AssetImporter.hpp"
#include "AssetManager.hpp"

#include "TextureImporter.hpp"
#include "SceneImporter.hpp"
#include "PrefabImporter.hpp"

namespace fox
{
    using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
    static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
        { AssetType::Texture, TextureImporter::ImportTexture2D },
        { AssetType::Scene, SceneImporter::ImportScene },
        { AssetType::Prefab, PrefabImporter::Import },
    };

    using AssetSerializeFunction = std::function<void(const AssetMetadata&, const Ref<Asset>&)>;
    static std::map<AssetType, AssetSerializeFunction> s_AssetSerializeFunctions = {
        { AssetType::Scene, SceneImporter::Serialize },
        { AssetType::Prefab, PrefabImporter::Serialize },
    };

    Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
    {
        if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
        {
            FOX_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
            return nullptr;
        }

        return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
    }
  
    void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
    {
        if (s_AssetSerializeFunctions.find(metadata.Type) == s_AssetSerializeFunctions.end())
        {
            FOX_CORE_WARN("There's currently no importer for assets of type %", metadata.FilePath.stem().string());
            return;
        }

        s_AssetSerializeFunctions.at(metadata.Type)(metadata, asset);
    }

    void AssetImporter::Serialize(const Ref<Asset>& asset)
    {
        const AssetMetadata& metadata = asset->GetMetadata();
        Serialize(metadata, asset);
    }
}
