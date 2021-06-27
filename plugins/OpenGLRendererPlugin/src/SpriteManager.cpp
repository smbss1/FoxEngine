//
//
//#include "SpriteManager.hpp"
//
//namespace fox
//{
//    SpriteManager::SpriteManager(ResourceManager& oResourceManager) : AAssetManager(oResourceManager)
//    {
//    }
//
//    bool SpriteManager::Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept
//    {
//        if (!Exists(strAssetName))
//        {
//            sf::Sprite* pSprite = new sf::Sprite;
//            sf::Texture* pTexture = m_oResourceManager.GetAsset<sf::Texture>(strAssetName);
//
//            if (!pSprite || !pTexture)
//                return false;
//
//            pSprite->setTexture(*pTexture);
//            m_vAssets.emplace(strAssetName, pSprite);
//            return true;
//        }
//        return false;
//    }
//
//    void* SpriteManager::Get(const std::string& strAssetName)
//    {
//        if (Exists(strAssetName))
//            return static_cast<void*>(m_vAssets[strAssetName].get());
//        return nullptr;
//    }
//
//    bool SpriteManager::Remove(const std::string& strAssetName) noexcept
//    {
//        if (Exists(strAssetName))
//        {
//            m_vAssets.erase(strAssetName);
//            return true;
//        }
//        return false;
//    }
//} // namespace fox