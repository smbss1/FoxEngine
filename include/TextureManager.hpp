
#pragma once

#include <SFML/Graphics.hpp>

#include "ResourceManager.hpp"

namespace Fox
{
	class TextureManager : public IAssetManager
	{
	public:
		using TexturePtr = std::unique_ptr<sf::Texture>;

	private:
		std::unordered_map<std::string, TexturePtr> assets;

	public:
		bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept override
		{
			if (!Exists(strAssetName))
			{
				sf::Texture* pTexture = new sf::Texture;

				//if the resource fails to load, then it adds a default "fail" resource
				if(!pTexture->loadFromFile(strAssetPath)) {
					sf::Texture* pFail = new sf::Texture;
					pFail->loadFromFile("_fail_.png");
					assets.emplace(strAssetName, pFail);
					return false;
				}
				else {
					assets.emplace(strAssetName, pTexture);
					return true;
				}
			}
			else
				return false;
		}

		bool Exists(const std::string& strAssetName) const
		{
			return assets.find(strAssetName) != assets.end();
		}

		void* Get(const std::string& strAssetName) override
		{
			if (Exists(strAssetName))
				return static_cast<void*>(assets[strAssetName].get());
			else
				return nullptr;
		}

		bool Remove(const std::string& strAssetName) noexcept override
		{
			if (Exists(strAssetName))
			{
				assets.erase(strAssetName);
				return true;
			}
			else
				return false;
		}
	};
} // namespace Fox