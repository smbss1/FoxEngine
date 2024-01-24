//
// Created by samuel on 22/10/22.
//

#ifndef FOX_LEXER_ASSETMANAGER_HPP
#define FOX_LEXER_ASSETMANAGER_HPP


#include "Utils/FileSystem.hpp"
#include "Hash.hpp"

#include "AssetImporter.hpp"
#include "AssetRegistry.hpp"
#include "Core/Project.hpp"

#include <map>
#include <unordered_map>

namespace fox
{
    class AssetManager
    {
    public:
//        using AssetsChangeEventFn = std::function<void(const std::vector<FileSystemChangedEvent>&)>;
    public:
        static void Init();
//        static void SetAssetChangeCallback(const AssetsChangeEventFn& callback);
        static void Shutdown();

        static const AssetMetadata& GetMetadata(AssetHandle handle);
        static const AssetMetadata& GetMetadata(const fs::path& filepath);
        static const AssetMetadata& GetMetadata(const Ref<Asset>& asset) { return GetMetadata(asset->Handle); }

        static fs::path GetFileSystemPath(const AssetMetadata& metadata) { return Project::AssetsDir() / metadata.FilePath; }
        static std::string GetFileSystemPathString(const AssetMetadata& metadata) { return GetFileSystemPath(metadata).string(); }
        static fs::path GetRelativePath(const fs::path& filepath);

        static AssetHandle GetAssetHandleFromFilePath(const fs::path& filepath);
        static bool IsAssetHandleValid(AssetHandle assetHandle) { return IsMemoryAsset(assetHandle) || GetMetadata(assetHandle).IsValid(); }

        static AssetType GetAssetTypeFromExtension(const std::string& extension);
        static const std::vector<std::string>& GetExtensionsFromAssetType(AssetType type);

        static AssetType GetAssetTypeFromPath(const fs::path& path);

        static AssetHandle ImportAsset(const fs::path& filepath);
        static bool ReloadData(AssetHandle assetHandle);

        template<typename T, typename... Args>
        static Ref<T> CreateNewAsset(const std::string& filename, const std::string& directoryPath, Args&&... args)
        {
            static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for types derived from Asset");

            AssetMetadata metadata;
            metadata.Handle = AssetHandle();
            if (directoryPath.empty() || directoryPath == ".")
                metadata.FilePath = filename;
            else
                metadata.FilePath = AssetManager::GetRelativePath(directoryPath + "/" + filename);
            metadata.IsDataLoaded = true;
            metadata.Type = T::GetStaticType();

            if (FileExists(metadata))
            {
                bool foundAvailableFileName = false;
                int current = 1;

                while (!foundAvailableFileName)
                {
                    std::string nextFilePath = directoryPath + "/" + metadata.FilePath.stem().string();

                    if (current < 10)
                        nextFilePath += " (0" + std::to_string(current) + ")";
                    else
                        nextFilePath += " (" + std::to_string(current) + ")";
                    nextFilePath += metadata.FilePath.extension().string();

                    if (!FileSystem::Exists(nextFilePath))
                    {
                        foundAvailableFileName = true;
                        metadata.FilePath = AssetManager::GetRelativePath(nextFilePath);
                        break;
                    }

                    current++;
                }
            }

            s_AssetRegistry[metadata.Handle] = metadata;

            WriteRegistryToFile();

            Ref<T> asset = Ref<T>::Create(std::forward<Args>(args)...);
            asset->Handle = metadata.Handle;
            s_LoadedAssets[asset->Handle] = asset;
            AssetImporter::Serialize(metadata, asset);

            return asset;
        }

        template<typename T>
        static Ref<T> GetAsset(AssetHandle assetHandle)
        {
//            FOX_PROFILE_FUNC();

            if (IsMemoryAsset(assetHandle))
                return s_MemoryAssets[assetHandle].As<T>();

            auto& metadata = GetMetadataInternal(assetHandle);
            if (!metadata.IsValid())
                return nullptr;

            Ref<Asset> asset = nullptr;
            if (!metadata.IsDataLoaded)
            {
                asset = AssetImporter::ImportAsset(assetHandle, metadata);
                metadata.IsDataLoaded = asset != nullptr;
                if (!metadata.IsDataLoaded)
                    return nullptr;

                asset->Handle = assetHandle;
                s_LoadedAssets[assetHandle] = asset;
            }
            else
            {
                asset = s_LoadedAssets[assetHandle];
            }

            return asset.As<T>();
        }

        template<typename T>
        static Ref<T> GetAsset(const std::string_view& filepath)
        {
            return GetAsset<T>(GetAssetHandleFromFilePath(filepath));
        }

        static bool FileExists(AssetMetadata& metadata)
        {
            return FileSystem::Exists(Project::GetActive()->AssetsDir() / metadata.FilePath);
        }

        static const std::unordered_map<AssetHandle, Ref<Asset>>& GetLoadedAssets() { return s_LoadedAssets; }
        static const AssetRegistry& GetAssetRegistry() { return s_AssetRegistry; }
        static const std::unordered_map<AssetHandle, Ref<Asset>>& GetMemoryOnlyAssets() { return s_MemoryAssets; }

        template<typename TAsset, typename... TArgs>
        static AssetHandle CreateMemoryOnlyAsset(TArgs&&... args)
        {
            static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

            Ref<TAsset> asset = Ref<TAsset>::Create(std::forward<TArgs>(args)...);
            asset->Handle = AssetHandle();

            s_MemoryAssets[asset->Handle] = asset;
            return asset->Handle;
        }

        template<typename TAsset, typename... TArgs>
        static AssetHandle CreateNamedMemoryOnlyAsset(const std::string& name, TArgs&&... args)
        {
            static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

            Ref<TAsset> asset = Ref<TAsset>::Create(std::forward<TArgs>(args)...);
            asset->Handle = Hash::FNVHash(name);

            AssetMetadata metadata;
            metadata.Handle = asset->Handle;
            metadata.FilePath = name;
            metadata.IsDataLoaded = true;
            metadata.Type = TAsset::GetStaticType();
            metadata.IsMemoryAsset = true;

            s_AssetRegistry[metadata.Handle] = metadata;

            s_MemoryAssets[asset->Handle] = asset;
            return asset->Handle;
        }

        static bool IsMemoryAsset(AssetHandle handle)
        {
            return s_MemoryAssets.find(handle) != s_MemoryAssets.end();
        }
    private:
        static void LoadAssetRegistry();
        static void ProcessDirectory(const fs::path& directoryPath);
        static void ReloadAssets();
        static void WriteRegistryToFile();

        static AssetMetadata& GetMetadataInternal(AssetHandle handle);

//        static void OnFileSystemChanged(const std::vector<FileSystemChangedEvent>& events);
        static void OnAssetRenamed(AssetHandle assetHandle, const fs::path& newFilePath);
        static void OnAssetDeleted(AssetHandle assetHandle);

    private:
        static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
        static std::unordered_map<AssetHandle, Ref<Asset>> s_MemoryAssets;
//        static AssetsChangeEventFn s_AssetsChangeCallback;
        inline static AssetRegistry s_AssetRegistry;
    private:
        friend class ContentBrowserPanel;
        friend class ContentBrowserAsset;
        friend class ContentBrowserDirectory;
    };

}



#endif //FOX_LEXER_ASSETMANAGER_HPP
