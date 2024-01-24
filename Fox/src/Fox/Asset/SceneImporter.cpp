#include "SceneImporter.hpp"

#include "Core/Project.hpp"
#include "Scene/SceneSerializer.hpp"
#include "asset/AssetManager.hpp"

#include <stb_image.h>

namespace fox
{

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		//FOX_PROFILE_FUNCTION();

		return LoadScene(Project::AssetsDir() / metadata.FilePath);
	}

	void SceneImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
    {
        SceneSerializer serializer(asset.As<Scene>());
        serializer.Serialize(AssetManager::GetFileSystemPath(metadata).string());
    }

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		//FOX_PROFILE_FUNCTION();

		Ref<Scene> scene = new_ref<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::AssetsDir() / path);
	}
}