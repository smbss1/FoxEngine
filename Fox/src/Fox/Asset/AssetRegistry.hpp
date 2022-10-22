//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_ASSETREGISTRY_HPP
#define FOXENGINE_ASSETREGISTRY_HPP

#include "AssetMetadata.hpp"

#include <unordered_map>

namespace fox
{
    class AssetRegistry
    {
    public:
        AssetMetadata& operator[](const AssetHandle handle);
        const AssetMetadata& Get(const AssetHandle handle) const;

        size_t Count() const { return m_AssetRegistry.size(); }
        bool Contains(const AssetHandle handle) const;
        size_t Remove(const AssetHandle handle);
        void Clear();

        auto begin() { return m_AssetRegistry.begin(); }
        auto end() { return m_AssetRegistry.end(); }
        auto begin() const { return m_AssetRegistry.cbegin(); }
        auto end() const { return m_AssetRegistry.cend(); }
    private:
        std::unordered_map<AssetHandle, AssetMetadata> m_AssetRegistry;
    };
}

#endif //FOX_LEXER_ASSETREGISTRY_HPP
