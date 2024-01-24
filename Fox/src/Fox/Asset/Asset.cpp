#include "Asset/Asset.hpp"
#include "Asset/AssetManager.hpp"
#include "Asset/AssetMetadata.hpp"

namespace fox
{
	const AssetMetadata& Asset::GetMetadata() const
	{
		return AssetManager::GetMetadata(Handle);
	}
}