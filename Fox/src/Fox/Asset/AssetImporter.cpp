//
// Created by samuel on 22/10/22.
//

#include "AssetImporter.hpp"
#include "AssetManager.hpp"

namespace fox
{
    std::unordered_map<AssetType, scope<AssetSerializer>> AssetImporter::s_Serializers;

    void AssetImporter::Init()
    {
        s_Serializers[AssetType::Prefab] = new_scope<PrefabSerializer>();
        s_Serializers[AssetType::Texture] = new_scope<TextureSerializer>();
//        s_Serializers[AssetType::Material] = new_scope<MaterialAssetSerializer>();
//        s_Serializers[AssetType::PhysicsMat] = new_scope<PhysicsMaterialSerializer>();
//        s_Serializers[AssetType::Audio] = new_scope<AudioFileSourceSerializer>();
//        s_Serializers[AssetType::SoundConfig] = new_scope<SoundConfigSerializer>();
        s_Serializers[AssetType::Scene] = new_scope<SceneAssetSerializer>();
//        s_Serializers[AssetType::Font] = new_scope<FontSerializer>();
//        s_Serializers[AssetType::SoundGraphSound] = new_scope<SoundGraphGraphSerializer>();
    }

    void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
    {
        if (s_Serializers.find(metadata.Type) == s_Serializers.end())
        {
            FOX_CORE_WARN("There's currently no importer for assets of type %", metadata.FilePath.stem().string());
            return;
        }

        s_Serializers[asset->GetAssetType()]->Serialize(metadata, asset);
    }

    void AssetImporter::Serialize(const Ref<Asset>& asset)
    {
        const AssetMetadata& metadata = AssetManager::GetMetadata(asset->Handle);
        Serialize(metadata, asset);
    }

    bool AssetImporter::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset)
    {
        if (s_Serializers.find(metadata.Type) == s_Serializers.end())
        {
            FOX_CORE_WARN("There's currently no importer for assets of type %", metadata.FilePath.stem().string());
            return false;
        }

        return s_Serializers[metadata.Type]->TryLoadData(metadata, asset);
    }
}
