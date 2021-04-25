
#ifndef FOX_SPRITE_MANAGER_HPP_
#define FOX_SPRITE_MANAGER_HPP_

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"

namespace fox
{
	class SpriteManager : public AAssetManager<sf::Sprite>
	{
	public:
	    SpriteManager(ResourceManager&);
		~SpriteManager();

		bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept override;
		void* Get(const std::string& strAssetName) override;
		bool Remove(const std::string& strAssetName) noexcept override;
	};
} // namespace Arcade

#endif