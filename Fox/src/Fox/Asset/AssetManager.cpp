//
// Created by samuel on 22/10/22.
//

#include "AssetManager.hpp"

#include "Renderer/Renderer2D.hpp"
#include "Core/Project.hpp"
#include "yaml-cpp/yaml.h"

#include "Core/Base.hpp"

#include "AssetExtensions.hpp"
#include "Utils/String.hpp"

namespace fox
{
    static AssetMetadata s_NullMetadata;

    void AssetManager::Init()
    {
        //AssetImporter::Init();

        LoadAssetRegistry();
//        FileSystem::SetChangeCallback(AssetManager::OnFileSystemChanged);
        ReloadAssets();
    }

//    void AssetManager::SetAssetChangeCallback(const AssetsChangeEventFn& callback)
//    {
//        s_AssetsChangeCallback = callback;
//    }

    void AssetManager::Shutdown()
    {
        WriteRegistryToFile();

        s_MemoryAssets.clear();
        s_AssetRegistry.Clear();
        s_LoadedAssets.clear();
    }

//    void AssetManager::OnFileSystemChanged(const std::vector<FileSystemChangedEvent>& events)
//    {
//        // Process all events before the refreshing the Content Browser
//        for (const auto& e : events)
//        {
//            if (!e.IsDirectory)
//            {
//                switch (e.Action)
//                {
//                    case FileSystemAction::Modified:
//                    {
//                        // TODO(Peter): This doesn't seem to work properly with MeshAssets.
//                        //				It reads in the new mesh data, but doesn't update the Mesh instances...
//                        //				This might be an issue with Refs but I'm not sure
//                        AssetHandle handle = GetAssetHandleFromFilePath(e.FilePath);
//                        const auto& metadata = GetMetadata(handle);
//
//                        // FIXME(Peter): This prevents the engine from crashing due to prefabs being modified on disk. This isn't the correct fix, since the issue is that we try to call mono functions from the watcher thread
//                        //					but this does work for now. This fix will 100% cause issues at some point in the future, but until Yan does the multi-threading pass of the engine, this will have to do.
//                        if (metadata.Type == AssetType::Prefab)
//                            break;
//
//                        if (metadata.IsValid() && metadata.IsDataLoaded)
//                            ReloadData(handle);
//                        break;
//                    }
//                    case FileSystemAction::Rename:
//                    {
//                        AssetType previousType = GetAssetTypeFromPath(e.OldName);
//                        AssetType newType = GetAssetTypeFromPath(e.FilePath);
//
//                        if (previousType == AssetType::None && newType != AssetType::None)
//                            ImportAsset(e.FilePath);
//                        else
//                            OnAssetRenamed(GetAssetHandleFromFilePath(e.FilePath.parent_path() / e.OldName), e.FilePath);
//                        break;
//                    }
//                    case FileSystemAction::Delete:
//                        OnAssetDeleted(GetAssetHandleFromFilePath(e.FilePath));
//                        break;
//                }
//            }
//        }
//
//        s_AssetsChangeCallback(events);
//    }

    AssetMetadata& AssetManager::GetMetadataInternal(AssetHandle handle)
    {
        if (s_AssetRegistry.Contains(handle))
            return s_AssetRegistry[handle];

        return s_NullMetadata; // make sure to check return value before you go changing it!
    }

    const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
    {
        if (s_AssetRegistry.Contains(handle))
            return s_AssetRegistry[handle];

        return s_NullMetadata;
    }

    const AssetMetadata& AssetManager::GetMetadata(const fs::path& filepath)
    {
        const auto relativePath = GetRelativePath(filepath);

        for (auto& [handle, metadata] : s_AssetRegistry)
        {
            if (metadata.FilePath == relativePath)
                return metadata;
        }

        return s_NullMetadata;
    }

    fs::path AssetManager::GetRelativePath(const fs::path& filepath)
    {
        fs::path relativePath = filepath.lexically_normal();
        std::string temp = filepath.string();
        if (temp.find(Project::AssetsDir().string()) != std::string::npos)
        {
            relativePath = fs::relative(filepath, Project::AssetsDir());
            if (relativePath.empty())
            {
                relativePath = filepath.lexically_normal();
            }
        }
        return relativePath;
    }

    AssetHandle AssetManager::GetAssetHandleFromFilePath(const fs::path& filepath)
    {
        return GetMetadata(filepath).Handle;
    }

    void AssetManager::OnAssetRenamed(AssetHandle assetHandle, const fs::path& newFilePath)
    {
        AssetMetadata metadata = GetMetadata(assetHandle);
        if (!metadata.IsValid())
            return;

        metadata.FilePath = GetRelativePath(newFilePath);
        WriteRegistryToFile();
    }

    void AssetManager::OnAssetDeleted(AssetHandle assetHandle)
    {
        AssetMetadata metadata = GetMetadata(assetHandle);
        if (!metadata.IsValid())
            return;

        s_AssetRegistry.Remove(assetHandle);
        s_LoadedAssets.erase(assetHandle);
        WriteRegistryToFile();
    }

    AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension)
    {
        std::string ext = Utils::String::ToLowerCopy(extension);
        if (s_AssetExtensionMap.find(ext) == s_AssetExtensionMap.end())
            return AssetType::None;

        return s_AssetExtensionMap.at(ext.c_str());
    }

    const std::vector<std::string>& AssetManager::GetExtensionsFromAssetType(AssetType type)
    {
        if (s_AssetTypeToExtensionsMap.find(type) == s_AssetTypeToExtensionsMap.end())
            return {};
        return s_AssetTypeToExtensionsMap.at(type);
    }

    AssetType AssetManager::GetAssetTypeFromPath(const fs::path& path)
    {
        return GetAssetTypeFromExtension(path.extension().string());
    }

    void AssetManager::LoadAssetRegistry()
    {
        FOX_CORE_INFO_TAG("AssetManager", "Loading Asset Registry: %", Project::AssetRegistryPath());

        const auto& assetRegistryPath = Project::AssetRegistryPath();
        if (!FileSystem::Exists(assetRegistryPath))
            return;

        std::ifstream stream(assetRegistryPath);
        FOX_CORE_ASSERT(stream);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        auto handles = data["Assets"];
        if (!handles)
        {
            FOX_CORE_ERROR("[AssetManager] Asset Registry appears to be corrupted!");
            FOX_CORE_VERIFY(false);
            return;
        }

        for (auto entry : handles)
        {
            std::string filepath = entry["FilePath"].as<std::string>();

            AssetMetadata metadata;
            metadata.Handle = entry["Handle"].as<uint64_t>();
            metadata.FilePath = filepath;
            metadata.Type = (AssetType)Utils::AssetTypeFromString(entry["Type"].as<std::string>());

            if (metadata.Type == AssetType::None)
                continue;

            if (!FileSystem::Exists(AssetManager::GetFileSystemPath(metadata)))
            {
                FOX_CORE_WARN("[AssetManager] Missing asset '%' detected in registry file, trying to locate...", metadata.FilePath);

                std::string mostLikelyCandidate;
                uint32_t bestScore = 0;

                for (auto& pathEntry : fs::recursive_directory_iterator(Project::AssetsDir()))
                {
                    const fs::path& path = pathEntry.path();

                    if (path.filename() != metadata.FilePath.filename())
                        continue;

                    if (bestScore > 0)
                        FOX_CORE_WARN("[AssetManager] Multiple candidates found...");

                    std::vector<std::string> candiateParts = Utils::String::SplitString(path.string(), "/\\");

                    uint32_t score = 0;
                    for (const auto& part : candiateParts)
                    {
                        if (filepath.find(part) != std::string::npos)
                            score++;
                    }

                    FOX_CORE_WARN("'%' has a score of %, best score is %", path.string(), score, bestScore);

                    if (bestScore > 0 && score == bestScore)
                    {
                        // TODO: How do we handle this?
                        // Probably prompt the user at this point?
                    }

                    if (score <= bestScore)
                        continue;

                    bestScore = score;
                    mostLikelyCandidate = path.string();
                }

                if (mostLikelyCandidate.empty() && bestScore == 0)
                {
                    FOX_CORE_ERROR("[AssetManager] Failed to locate a potential match for '%'", metadata.FilePath);
                    continue;
                }

                std::replace(mostLikelyCandidate.begin(), mostLikelyCandidate.end(), '\\', '/');
                metadata.FilePath = fs::relative(mostLikelyCandidate, Project::GetActive()->AssetsDir());
                FOX_CORE_WARN("[AssetManager] Found most likely match '%'", metadata.FilePath);
            }

            if (metadata.Handle == 0)
            {
                FOX_CORE_WARN("[AssetManager] AssetHandle for % is 0, this shouldn't happen.", metadata.FilePath);
                continue;
            }

            s_AssetRegistry[metadata.Handle] = metadata;
        }

        FOX_CORE_INFO("[AssetManager] Loaded % asset entries", s_AssetRegistry.Count());
    }

    AssetHandle AssetManager::ImportAsset(const fs::path& filepath)
    {
        fs::path path = GetRelativePath(filepath);

        if(auto& metadata = GetMetadata(path); metadata.IsValid())
            return metadata.Handle;

        AssetType type = GetAssetTypeFromPath(path);
        if (type == AssetType::None)
            return 0;

        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.FilePath = path;
        metadata.Type = type;
        s_AssetRegistry[metadata.Handle] = metadata;

        return metadata.Handle;
    }

    bool AssetManager::ReloadData(AssetHandle assetHandle)
    {
        auto& metadata = GetMetadataInternal(assetHandle);
        if (!metadata.IsValid())
        {
            FOX_CORE_ERROR("Trying to reload invalid asset");
            return false;
        }

        if (!metadata.IsDataLoaded)
        {
            FOX_CORE_WARN("Trying to reload asset that was never loaded");

            Ref<Asset> asset = AssetImporter::ImportAsset(metadata.Handle, metadata);
            if (asset)
            {
                asset->Handle = assetHandle;
            }
            metadata.IsDataLoaded = asset != nullptr;
            return metadata.IsDataLoaded;
        }

        FOX_CORE_ASSERT(s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end());
        Ref<Asset>& asset = s_LoadedAssets.at(assetHandle);
        asset = AssetImporter::ImportAsset(metadata.Handle, metadata);
        if (asset)
        {
            asset->Handle = assetHandle;
        }
        metadata.IsDataLoaded = asset != nullptr;
        return metadata.IsDataLoaded;
    }

    void AssetManager::ProcessDirectory(const fs::path& directoryPath)
    {
        for (auto entry : fs::directory_iterator(directoryPath))
        {
            if (entry.is_directory())
                ProcessDirectory(entry.path());
            else
                ImportAsset(entry.path());
        }
    }

    void AssetManager::ReloadAssets()
    {
        ProcessDirectory(Project::AssetsDir().string());
        WriteRegistryToFile();
    }

    void AssetManager::WriteRegistryToFile()
    {
        // Sort assets by UUID to make project managment easier
        struct AssetRegistryEntry
        {
            std::string FilePath;
            AssetType Type;
        };
        std::map<UUID, AssetRegistryEntry> sortedMap;
        for (auto& [filepath, metadata] : s_AssetRegistry)
        {
            if (!FileSystem::Exists(GetFileSystemPath(metadata)))
                continue;

            if (metadata.IsMemoryAsset)
                continue;

            std::string pathToSerialize = metadata.FilePath.string();
            // NOTE(Yan): if Windows
            std::replace(pathToSerialize.begin(), pathToSerialize.end(), '\\', '/');
            sortedMap[metadata.Handle] = { pathToSerialize, metadata.Type };
        }

        FOX_CORE_INFO_TAG("AssetManager", "Serializing asset registry with % entries", sortedMap.size());

        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Assets" << YAML::BeginSeq;
        for (auto& [handle, entry] : sortedMap)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Handle" << YAML::Value << handle;
            out << YAML::Key << "FilePath" << YAML::Value << entry.FilePath;
            out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(entry.Type);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

//        FileSystem::SkipNextFileSystemChange();

        const std::string& assetRegistryPath = Project::AssetRegistryPath().string();
        std::ofstream fout(assetRegistryPath);
        fout << out.c_str();
    }

    std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_LoadedAssets;
    std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_MemoryAssets;

//    AssetManager::AssetsChangeEventFn AssetManager::s_AssetsChangeCallback;

}
