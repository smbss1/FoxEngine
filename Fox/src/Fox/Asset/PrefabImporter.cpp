#include "PrefabImporter.hpp"

#include "Core/Project.hpp"
#include "asset/AssetManager.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Scene/Prefab.hpp"

namespace fox
{
	Ref<Asset> PrefabImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
	{
		//FOX_PROFILE_FUNCTION();

		Ref<Prefab> prefab = new_ref<Prefab>();
		EntitySerializer::DeserializeEntityAsPrefab(AssetManager::GetFileSystemPath(metadata), *prefab->m_Scene);
		return prefab;
	}

	void PrefabImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
    {
		// TODO
    }
}