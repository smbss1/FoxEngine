#include "ThumbnailCache.hpp"

#include "Asset/TextureImporter.hpp"

#include <chrono>

namespace fox
{
	ThumbnailCache::ThumbnailCache()
	{
		// TODO(Yan): revisit this path (move to Cache dir)
		// m_ThumbnailCachePath = m_Project->AssetsDir() / "Thumbnail.cache";
	}

	Ref<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path& assetPath)
	{
		if (!Project::GetActive())
			return nullptr;

		// 1. Read file timestamp
		// 2. Compare hashed timestamp with existing cached image (in memory first, then from cache file)
		// 3. If equal, return associated thumbnail, otherwise load asset from disk and generate thumbnail
		// 4. If generated new thumbnail, store in cache obviously

		auto absolutePath = Project::GetAssetAbsolutePath(assetPath);
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(absolutePath);
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

		if (m_CachedImages.find(assetPath) != m_CachedImages.end())
		{
			auto& cachedImage = m_CachedImages.at(assetPath);
			if (cachedImage.Timestamp == timestamp)
				return cachedImage.Image;
		}

		// TODO(Yan): PNGs for now
		if (assetPath.extension() != ".png")
			return nullptr;

		Ref<Texture2D> texture = TextureImporter::LoadTexture2D(absolutePath.string());
		if (!texture)
			return nullptr;

		auto& cachedImage = m_CachedImages[assetPath];
		cachedImage.Timestamp = timestamp;
		cachedImage.Image = texture;
		return cachedImage.Image;
	}

}