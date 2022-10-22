//
// Created by samuel on 22/10/22.
//

#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include "AssetSerializer.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Scene/SceneSerializer.hpp"
#include "Renderer/Texture.hpp"
#include "Ecs/Entity.hpp"
#include "Components/IDComponent.hpp"
#include "Scene/Prefab.hpp"
#include "AssetManager.hpp"

namespace fox
{
    bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
    {
        asset = Texture2D::Create(AssetManager::GetFileSystemPathString(metadata));
        asset->Handle = metadata.Handle;

        bool result = asset.As<Texture2D>()->IsLoaded();

        if (!result)
            asset->SetFlag(AssetFlag::Invalid, true);

        return result;
    }
//
//    bool FontSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
//    {
//        asset = Ref<Font>::Create(AssetManager::GetFileSystemPathString(metadata));
//        asset->Handle = metadata.Handle;
//
//#if 0
//        // TODO(Yan): we should probably handle fonts not loading correctly
//		bool result = asset.As<Font>()->Loaded();
//		if (!result)
//			asset->SetFlag(AssetFlag::Invalid, true);
//#endif
//
//        return true;
//    }
//
//    void MaterialAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
//    {
//        Ref<MaterialAsset> material = asset.As<MaterialAsset>();
//
//        YAML::Emitter out;
//        out << YAML::BeginMap; // Material
//        out << YAML::Key << "Material" << YAML::Value;
//        {
//            out << YAML::BeginMap;
//
//            // TODO(Yan): this should have shader UUID when that's a thing
//            //            right now only supports PBR or Transparent shaders
//            Ref<Shader> transparentShader = Renderer::GetShaderLibrary()->Get("HazelPBR_Transparent");
//            bool transparent = material->GetMaterial()->GetShader() == transparentShader;
//            HZ_SERIALIZE_PROPERTY(Transparent, transparent, out);
//
//            HZ_SERIALIZE_PROPERTY(AlbedoColor, material->GetAlbedoColor(), out);
//            HZ_SERIALIZE_PROPERTY(Emission, material->GetEmission(), out);
//            if (!transparent)
//            {
//                HZ_SERIALIZE_PROPERTY(UseNormalMap, material->IsUsingNormalMap(), out);
//                HZ_SERIALIZE_PROPERTY(Metalness, material->GetMetalness(), out);
//                HZ_SERIALIZE_PROPERTY(Roughness, material->GetRoughness(), out);
//            }
//            else
//            {
//                HZ_SERIALIZE_PROPERTY(Transparency, material->GetTransparency(), out);
//            }
//
//            {
//                Ref<Texture2D> albedoMap = material->GetAlbedoMap();
//                bool hasAlbedoMap = albedoMap ? !albedoMap.EqualsObject(Renderer::GetWhiteTexture()) : false;
//                AssetHandle albedoMapHandle = hasAlbedoMap ? albedoMap->Handle : 0;
//                HZ_SERIALIZE_PROPERTY(AlbedoMap, albedoMapHandle, out);
//            }
//            if (!transparent)
//            {
//                {
//                    Ref<Texture2D> normalMap = material->GetNormalMap();
//                    bool hasNormalMap = normalMap ? !normalMap.EqualsObject(Renderer::GetWhiteTexture()) : false;
//                    AssetHandle normalMapHandle = hasNormalMap ? normalMap->Handle : 0;
//                    HZ_SERIALIZE_PROPERTY(NormalMap, normalMapHandle, out);
//                }
//                {
//                    Ref<Texture2D> metalnessMap = material->GetMetalnessMap();
//                    bool hasMetalnessMap = metalnessMap ? !metalnessMap.EqualsObject(Renderer::GetWhiteTexture()) : false;
//                    AssetHandle metalnessMapHandle = hasMetalnessMap ? metalnessMap->Handle : 0;
//                    HZ_SERIALIZE_PROPERTY(MetalnessMap, metalnessMapHandle, out);
//                }
//                {
//                    Ref<Texture2D> roughnessMap = material->GetRoughnessMap();
//                    bool hasRoughnessMap = roughnessMap ? !roughnessMap.EqualsObject(Renderer::GetWhiteTexture()) : false;
//                    AssetHandle roughnessMapHandle = hasRoughnessMap ? roughnessMap->Handle : 0;
//                    HZ_SERIALIZE_PROPERTY(RoughnessMap, roughnessMapHandle, out);
//                }
//            }
//
//            HZ_SERIALIZE_PROPERTY(MaterialFlags, material->GetMaterial()->GetFlags(), out);
//
//            out << YAML::EndMap;
//        }
//        out << YAML::EndMap; // Material
//
//        std::ofstream fout(AssetManager::GetFileSystemPath(metadata));
//        fout << out.c_str();
//    }
//
//    bool MaterialAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
//    {
//        std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
//        if (!stream.is_open())
//            return false;
//
//        std::stringstream strStream;
//        strStream << stream.rdbuf();
//
//        YAML::Node root = YAML::Load(strStream.str());
//        YAML::Node materialNode = root["Material"];
//
//        bool transparent = false;
//        HZ_DESERIALIZE_PROPERTY(Transparent, transparent, materialNode, false);
//
//        Ref<MaterialAsset> material = Ref<MaterialAsset>::Create(transparent);
//
//        HZ_DESERIALIZE_PROPERTY(AlbedoColor, material->GetAlbedoColor(), materialNode, glm::vec3(0.8f));
//        HZ_DESERIALIZE_PROPERTY(Emission, material->GetEmission(), materialNode, 0.0f);
//
//        if (!transparent)
//        {
//            material->SetUseNormalMap(materialNode["UseNormalMap"] ? materialNode["UseNormalMap"].as<bool>() : false);
//            HZ_DESERIALIZE_PROPERTY(Metalness, material->GetMetalness(), materialNode, 0.0f);
//            HZ_DESERIALIZE_PROPERTY(Roughness, material->GetRoughness(), materialNode, 0.5f);
//        }
//        else
//        {
//            HZ_DESERIALIZE_PROPERTY(Transparency, material->GetTransparency(), materialNode, 1.0f);
//        }
//
//        AssetHandle albedoMap, normalMap, metalnessMap, roughnessMap;
//        HZ_DESERIALIZE_PROPERTY(AlbedoMap, albedoMap, materialNode, (AssetHandle)0);
//        if (!transparent)
//        {
//            HZ_DESERIALIZE_PROPERTY(NormalMap, normalMap, materialNode, (AssetHandle)0);
//            HZ_DESERIALIZE_PROPERTY(MetalnessMap, metalnessMap, materialNode, (AssetHandle)0);
//            HZ_DESERIALIZE_PROPERTY(RoughnessMap, roughnessMap, materialNode, (AssetHandle)0);
//        }
//        if (albedoMap)
//        {
//            if (AssetManager::IsAssetHandleValid(albedoMap))
//                material->SetAlbedoMap(AssetManager::GetAsset<Texture2D>(albedoMap));
//        }
//        if (normalMap)
//        {
//            if (AssetManager::IsAssetHandleValid(normalMap))
//                material->SetNormalMap(AssetManager::GetAsset<Texture2D>(normalMap));
//        }
//        if (metalnessMap)
//        {
//            if (AssetManager::IsAssetHandleValid(metalnessMap))
//                material->SetMetalnessMap(AssetManager::GetAsset<Texture2D>(metalnessMap));
//        }
//        if (roughnessMap)
//        {
//            if (AssetManager::IsAssetHandleValid(roughnessMap))
//                material->SetRoughnessMap(AssetManager::GetAsset<Texture2D>(roughnessMap));
//        }
//
//        HZ_DESERIALIZE_PROPERTY(MaterialFlags, roughnessMap, materialNode, (AssetHandle)0);
//        if (materialNode["MaterialFlags"])
//            material->GetMaterial()->SetFlags(materialNode["MaterialFlags"].as<uint32_t>());
//
//        asset = material;
//        asset->Handle = metadata.Handle;
//
//        return true;
//    }
//
//    void PhysicsMaterialSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
//    {
//        Ref<PhysicsMaterial> material = asset.As<PhysicsMaterial>();
//
//        YAML::Emitter out;
//
//        out << YAML::BeginMap;
//        out << YAML::Key << "StaticFriction" << material->StaticFriction;
//        out << YAML::Key << "DynamicFriction" << material->DynamicFriction;
//        out << YAML::Key << "Bounciness" << material->Bounciness;
//        out << YAML::EndMap;
//
//        std::ofstream fout(AssetManager::GetFileSystemPath(metadata));
//        fout << out.c_str();
//    }
//
//    bool PhysicsMaterialSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
//    {
//        std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
//        if (!stream.is_open())
//            return false;
//
//        std::stringstream strStream;
//        strStream << stream.rdbuf();
//
//        YAML::Node data = YAML::Load(strStream.str());
//
//        float staticFriction = data["StaticFriction"] ? data["StaticFriction"].as<float>() : 0.6f;
//        float dynamicFriction = data["DynamicFriction"] ? data["DynamicFriction"].as<float>() : 0.6f;
//        float bounciness = data["Bounciness"] ? data["Bounciness"].as<float>() : 0.0f;
//
//        asset = Ref<PhysicsMaterial>::Create(staticFriction, dynamicFriction, bounciness);
//        asset->Handle = metadata.Handle;
//
//        return true;
//    }
//
//    void AudioFileSourceSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
//    {
//
//    }
//
//    bool AudioFileSourceSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
//    {
//        if (auto opt = AudioFileUtils::GetFileInfo(metadata))
//        {
//            AudioFileUtils::AudioFileInfo info = opt.value();
//            asset = Ref<AudioFile>::Create(info.Duration, info.SamplingRate, info.BitDepth, info.NumChannels, info.FileSize);
//        }
//        else
//            asset = Ref<AudioFile>::Create();
//
//        asset->Handle = metadata.Handle;
//
//        return true;
//    }
//
//    void SoundConfigSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
//    {
//        Ref<SoundConfig> soundConfig = asset.As<SoundConfig>();
//
//        YAML::Emitter out;
//
//        out << YAML::BeginMap; // SoundConfig
//        if (soundConfig->DataSourceAsset)
//            HZ_SERIALIZE_PROPERTY(AssetID,		soundConfig->DataSourceAsset, out);
//
//        HZ_SERIALIZE_PROPERTY(IsLooping,		(bool)soundConfig->bLooping,	out);
//        HZ_SERIALIZE_PROPERTY(VolumeMultiplier,	soundConfig->VolumeMultiplier,	out);
//        HZ_SERIALIZE_PROPERTY(PitchMultiplier,	soundConfig->PitchMultiplier,	out);
//        HZ_SERIALIZE_PROPERTY(MasterReverbSend, soundConfig->MasterReverbSend,	out);
//        HZ_SERIALIZE_PROPERTY(LPFilterValue,	soundConfig->LPFilterValue,		out);
//        HZ_SERIALIZE_PROPERTY(HPFilterValue,	soundConfig->HPFilterValue,		out);
//
//        // TODO: move Spatialization to its own asset type
//        out << YAML::Key << "Spatialization";
//        out << YAML::BeginMap; // Spatialization
//
//        auto& spatialConfig = soundConfig->Spatialization;
//        HZ_SERIALIZE_PROPERTY(Enabled,				soundConfig->bSpatializationEnabled,	out);
//        HZ_SERIALIZE_PROPERTY(AttenuationModel,		(int)spatialConfig->AttenuationMod,		out);
//        HZ_SERIALIZE_PROPERTY(MinGain,				spatialConfig->MinGain,					out);
//        HZ_SERIALIZE_PROPERTY(MaxGain,				spatialConfig->MaxGain,					out);
//        HZ_SERIALIZE_PROPERTY(MinDistance,			spatialConfig->MinDistance,				out);
//        HZ_SERIALIZE_PROPERTY(MaxDistance,			spatialConfig->MaxDistance,				out);
//        HZ_SERIALIZE_PROPERTY(ConeInnerAngle,		spatialConfig->ConeInnerAngleInRadians,	out);
//        HZ_SERIALIZE_PROPERTY(ConeOuterAngle,		spatialConfig->ConeOuterAngleInRadians,	out);
//        HZ_SERIALIZE_PROPERTY(ConeOuterGain,		spatialConfig->ConeOuterGain,			out);
//        HZ_SERIALIZE_PROPERTY(DopplerFactor,		spatialConfig->DopplerFactor,			out);
//        HZ_SERIALIZE_PROPERTY(Rollor,				spatialConfig->Rolloff,					out);
//        HZ_SERIALIZE_PROPERTY(AirAbsorptionEnabled, spatialConfig->bAirAbsorptionEnabled,	out);
//        HZ_SERIALIZE_PROPERTY(SpreadFromSourceSize, spatialConfig->bSpreadFromSourceSize,	out);
//        HZ_SERIALIZE_PROPERTY(SourceSize,			spatialConfig->SourceSize,				out);
//        HZ_SERIALIZE_PROPERTY(Spread,				spatialConfig->Spread,					out);
//        HZ_SERIALIZE_PROPERTY(Focus,				spatialConfig->Focus,					out);
//
//        out << YAML::EndMap; // Spatialization
//        out << YAML::EndMap; // SoundConfig
//
//        std::ofstream fout(AssetManager::GetFileSystemPath(metadata));
//        fout << out.c_str();
//    }
//
//    bool SoundConfigSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
//    {
//        std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
//        if (!stream.is_open())
//            return false;
//
//
//        std::stringstream strStream;
//        strStream << stream.rdbuf();
//
//        YAML::Node data = YAML::Load(strStream.str());
//
//        auto soundConfig = Ref<SoundConfig>::Create();
//
//
//        AssetHandle assetHandle = data["AssetID"] ? data["AssetID"].as<uint64_t>() : 0;
//
//        if (AssetManager::IsAssetHandleValid(assetHandle))
//            soundConfig->DataSourceAsset = assetHandle;
//        else if (assetHandle != 0)
//            HZ_CORE_ERROR("Tried to load invalid Audio File asset in SoundConfig: %", metadata.FilePath);
//
//        HZ_DESERIALIZE_PROPERTY(IsLooping,			soundConfig->bLooping,			data, false);
//        HZ_DESERIALIZE_PROPERTY(VolumeMultiplier,	soundConfig->VolumeMultiplier,	data, 1.0f);
//        HZ_DESERIALIZE_PROPERTY(PitchMultiplier,	soundConfig->PitchMultiplier,	data, 1.0f);
//        HZ_DESERIALIZE_PROPERTY(MasterReverbSend,	soundConfig->MasterReverbSend,	data, 0.0f);
//        HZ_DESERIALIZE_PROPERTY(LPFilterValue,		soundConfig->LPFilterValue,		data, 20000.0f);
//        HZ_DESERIALIZE_PROPERTY(HPFilterValue,		soundConfig->HPFilterValue,		data, 0.0f);
//
//        auto spConfigData = data["Spatialization"];
//        if (spConfigData)
//        {
//            soundConfig->bSpatializationEnabled = spConfigData["Enabled"] ? spConfigData["Enabled"].as<bool>() : false;
//
//            auto& spatialConfig = soundConfig->Spatialization;
//
//            HZ_DESERIALIZE_PROPERTY(Enabled, soundConfig->bSpatializationEnabled				, spConfigData, false);
//            spatialConfig->AttenuationMod = spConfigData["AttenuationModel"] ? static_cast<AttenuationModel>(spConfigData["AttenuationModel"].as<int>())
//                                                                             : AttenuationModel::Inverse;
//
//            HZ_DESERIALIZE_PROPERTY(MinGain,				spatialConfig->MinGain,					spConfigData, 0.0f);
//            HZ_DESERIALIZE_PROPERTY(MaxGain,				spatialConfig->MaxGain,					spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(MinDistance,			spatialConfig->MinDistance,				spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(MaxDistance,			spatialConfig->MaxDistance,				spConfigData, 1000.0f);
//            HZ_DESERIALIZE_PROPERTY(ConeInnerAngle,			spatialConfig->ConeInnerAngleInRadians,	spConfigData, 6.283185f);
//            HZ_DESERIALIZE_PROPERTY(ConeOuterAngle,			spatialConfig->ConeOuterAngleInRadians,	spConfigData, 6.283185f);
//            HZ_DESERIALIZE_PROPERTY(ConeOuterGain,			spatialConfig->ConeOuterGain,			spConfigData, 0.0f);
//            HZ_DESERIALIZE_PROPERTY(DopplerFactor,			spatialConfig->DopplerFactor,			spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(Rollor,					spatialConfig->Rolloff,					spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(AirAbsorptionEnabled,	spatialConfig->bAirAbsorptionEnabled,	spConfigData, true);
//            HZ_DESERIALIZE_PROPERTY(SpreadFromSourceSize,	spatialConfig->bSpreadFromSourceSize,	spConfigData, true);
//            HZ_DESERIALIZE_PROPERTY(SourceSize,				spatialConfig->SourceSize,				spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(Spread,					spatialConfig->Spread,					spConfigData, 1.0f);
//            HZ_DESERIALIZE_PROPERTY(Focus,					spatialConfig->Focus,					spConfigData, 1.0f);
//        }
//
//        asset = soundConfig;
//        asset->Handle = metadata.Handle;
//
//        return true;
//    }

    void PrefabSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
    {
//        Ref<Prefab> prefab = asset.As<Prefab>();
//
//        YAML::Emitter out;
//
//        out << YAML::BeginMap;
//        out << YAML::Key << "Prefab";
//        out << YAML::Value << YAML::BeginSeq;
//
//        prefab->m_Scene->m_Registry.each([&](auto entityID)
//                                         {
//                                             Entity entity = { entityID, prefab->m_Scene.Raw() };
//                                             if (!entity || !entity.has<IDComponent>())
//                                                 return;
//
//                                             EntitySerializer::SerializeAllEntityComponents(out, entity);
//                                         });
//
//        out << YAML::EndSeq;
//        out << YAML::EndMap;
//
//        std::ofstream fout(AssetManager::GetFileSystemPath(metadata));
//        fout << out.c_str();
    }

    bool PrefabSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
    {
        Ref<Prefab> prefab = new_ref<Prefab>();
        EntitySerializer::DeserializeEntityAsPrefab(AssetManager::GetFileSystemPath(metadata).c_str(), *prefab->m_Scene);
        asset = prefab;
        asset->Handle = metadata.Handle;
        return true;
    }

    void SceneAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
    {
        SceneSerializer serializer(asset.As<Scene>());
        serializer.Serialize(AssetManager::GetFileSystemPath(metadata).string());
    }

    bool SceneAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
    {
        asset = new_ref<Asset>();
        asset->Handle = metadata.Handle;
        return true;
    }
}
