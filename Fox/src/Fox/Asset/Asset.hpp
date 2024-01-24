//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSET_HPP
#define FOXENGINE_ASSET_HPP


#include "Core/UUID.hpp"
#include "Asset/AssetTypes.hpp"

namespace fox
{
    using AssetHandle = UUID;
    struct AssetMetadata;

    struct AssetLink
    {
        AssetHandle Handle;
        uint16_t Type;
    };

    class Asset : public RefCounted
    {
    public:
        AssetHandle Handle = 0;
        uint16_t Flags = (uint16_t)AssetFlag::None;

        virtual ~Asset() {}

        static AssetType GetStaticType() { return AssetType::None; }
        virtual AssetType GetAssetType() const { return AssetType::None; }

        bool IsValid() const { return ((Flags & (uint16_t)AssetFlag::Missing) | (Flags & (uint16_t)AssetFlag::Invalid)) == 0; }

        const AssetMetadata& GetMetadata() const;

        virtual bool operator==(const Asset& other) const
        {
            return Handle == other.Handle;
        }

        virtual bool operator!=(const Asset& other) const
        {
            return !(*this == other);
        }

        bool IsFlagSet(AssetFlag flag) const { return (uint16_t)flag & Flags; }
        void SetFlag(AssetFlag flag, bool value = true)
        {
            if (value)
                Flags |= (uint16_t)flag;
            else
                Flags &= ~(uint16_t)flag;
        }
    };
}

#endif //FOXENGINE_ASSET_HPP
