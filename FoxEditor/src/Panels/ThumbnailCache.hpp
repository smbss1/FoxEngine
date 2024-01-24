
#ifndef FOXENGINE_THUMBNAILCACHE_HPP
#define FOXENGINE_THUMBNAILCACHE_HPP

#include "Core/Project.hpp"
#include "Renderer/Texture.hpp"

namespace fox
{
	struct ThumbnailImage
	{
		uint64_t Timestamp;
		Ref<Texture2D> Image;
	};

	class ThumbnailCache : public RefCounted
	{
	public:
		ThumbnailCache();

		Ref<Texture2D> GetOrCreateThumbnail(const std::filesystem::path& path);
	private:

		std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;

		// TEMP (replace with Hazel::Serialization)
		std::filesystem::path m_ThumbnailCachePath;
	};

}

#endif