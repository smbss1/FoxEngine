

#ifndef FOX_SFML_GRAPHICS_HPP_
#define FOX_SFML_GRAPHICS_HPP_

#include <SFML/Graphics.hpp>

#include "Core/Managers/ResourceManager.hpp"

namespace fox
{
	class TextureManager : public AAssetManager<sf::Texture>
	{
	public:
		explicit TextureManager(ResourceManager&);
		~TextureManager() override = default;
	
		bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept override;
		void* Get(const std::string& strAssetName) override;
		bool Remove(const std::string& strAssetName) noexcept override;
	};
} // namespace Fox

#endif