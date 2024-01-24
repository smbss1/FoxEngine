#pragma once

#include "Asset.hpp"
#include "AssetMetadata.hpp"

#include "Scene/Scene.hpp"

namespace fox
{
	class PrefabImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Asset> Import(AssetHandle handle, const AssetMetadata& metadata);
		static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);
	};
}