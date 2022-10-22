//
// Created by samuel on 22/10/22.
//

#include "AssetRegistry.hpp"

namespace fox
{
#define FOX_ASSETREGISTRY_LOG 0
#if FOX_ASSETREGISTRY_LOG
#define ASSET_LOG(...) FOX_CORE_TRACE_TAG("ASSET", __VA_ARGS__)
#else
#define ASSET_LOG(...)
#endif

    AssetMetadata& AssetRegistry::operator[](const AssetHandle handle)
    {
        ASSET_LOG("Retrieving handle {}", handle);
        return m_AssetRegistry[handle];
    }

    const AssetMetadata& AssetRegistry::Get(const AssetHandle handle) const
    {
        FOX_CORE_ASSERT(m_AssetRegistry.find(handle) != m_AssetRegistry.end());
        ASSET_LOG("Retrieving const handle {}", handle);
        return m_AssetRegistry.at(handle);
    }

    bool AssetRegistry::Contains(const AssetHandle handle) const
    {
        ASSET_LOG("Contains handle {}", handle);
        return m_AssetRegistry.find(handle) != m_AssetRegistry.end();
    }

    size_t AssetRegistry::Remove(const AssetHandle handle)
    {
        ASSET_LOG("Removing handle", handle);
        return m_AssetRegistry.erase(handle);
    }

    void AssetRegistry::Clear()
    {
        ASSET_LOG("Clearing registry");
        m_AssetRegistry.clear();
    }
}
