
#include "TextureManager.hpp"
#include <iostream>
namespace fox
{
	TextureManager::TextureManager(ResourceManager& oResourceManager) : AAssetManager(oResourceManager)
	{
	}

	bool TextureManager::Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept
	{
		if (!Exists(strAssetName))
		{
			sf::Texture* pTexture = new sf::Texture;

			//if the resource fails to load, then it adds a default "fail" resource
			if(!pTexture->loadFromFile(strAssetPath)) {
				pTexture->loadFromFile("_fail_.png");
				m_vAssets.emplace(strAssetName, pTexture);
				return false;
			} else {
				m_vAssets.emplace(strAssetName, pTexture);
				return true;
			}
		}
		return false;
	}

	void* TextureManager::Get(const std::string& strAssetName)
	{
		if (Exists(strAssetName))
			return static_cast<void*>(m_vAssets[strAssetName].get());
		return nullptr;
	}

	bool TextureManager::Remove(const std::string& strAssetName) noexcept
	{
		if (!Exists(strAssetName))
			return false;
		m_vAssets.erase(strAssetName);
		return true;
	}
} // namespace Fox