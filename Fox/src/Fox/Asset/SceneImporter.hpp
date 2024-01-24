#pragma once

#include "Asset.hpp"
#include "AssetMetadata.hpp"

#include "Scene/Scene.hpp"

namespace fox
{
	class SceneImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);

		// Load from filepath
		static Ref<Scene> LoadScene(const std::filesystem::path& path);

		static void SaveScene(Ref<Scene> scene, const std::filesystem::path& path);
	};
}