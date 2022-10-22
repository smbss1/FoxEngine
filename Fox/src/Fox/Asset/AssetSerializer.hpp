//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETSERIALIZER_HPP
#define FOXENGINE_ASSETSERIALIZER_HPP

#include "AssetMetadata.hpp"

namespace fox
{
    class AssetSerializer
    {
    public:
        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
    };

    class TextureSerializer : public AssetSerializer
    {
    public:
        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override{}
        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
    };

//    class FontSerializer : public AssetSerializer
//    {
//    public:
//        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
//        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
//    };

//    class MaterialAssetSerializer : public AssetSerializer
//    {
//    public:
//        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
//        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
//    };

//    class PhysicsMaterialSerializer : public AssetSerializer
//    {
//    public:
//        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
//        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
//    };

//    class AudioFileSourceSerializer : public AssetSerializer
//    {
//    public:
//        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
//        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
//    };
//
//    class SoundConfigSerializer : public AssetSerializer
//    {
//    public:
//        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
//        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
//    };

    class PrefabSerializer : public AssetSerializer
    {
    public:
        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
    };

    class SceneAssetSerializer : public AssetSerializer
    {
    public:
        virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
        virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
    };
}


#endif //FOXENGINE_ASSETSERIALIZER_HPP
