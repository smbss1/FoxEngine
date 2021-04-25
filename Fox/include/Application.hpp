
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include "FoxEcs.hpp"
#include "SceneManager.hpp"
#include "ResourceManager.hpp"
#include "Core.hpp"

namespace fox
{
    class FOX_API Application
    {
    public:
        Application(int ac, char** av);
        virtual ~Application();
    
        virtual void init();
        void run();

        // -------- Scene Manager ----------------------------------
        SceneId add_scene(ref<Scene> pScene);
        void switch_scene(SceneId id);
        void remove_scene(SceneId id);
        ref<Scene> get_active()
        {
            return m_pSceneManager->get_active();
        }
        // ---------------------------------------------------------------------

        // -------- Resource Manager ----------------------------------
        template <typename T>
		bool add_manager(IAssetManager* pAssetManager) noexcept
		{
			return m_pResourceManager->AddManager<T>(pAssetManager);
		}

		template <typename T>
		bool add_asset(const std::string& strAssetName, const std::string& strAssetPath) noexcept
		{
			return m_pResourceManager->AddAsset<T>(strAssetName, strAssetPath);
		}

		template<typename TAsset>
		TAsset* get_asset(const std::string& strAssetName)
		{
			return m_pResourceManager->GetAsset<TAsset>(strAssetName);
		}

		template<typename TAsset>
		bool remove_asset(const std::string& strAssetName) noexcept
		{
			return m_pResourceManager->RemoveAsset<TAsset>(strAssetName);
		}

        ResourceManager& get_resource_manager() const
        {
            return *m_pResourceManager;
        }
        // ---------------------------------------------------------------------

    private:
        bool m_bIsRunning;

    protected:
        scope<SceneManager> m_pSceneManager;
        scope<ResourceManager> m_pResourceManager;
    };
}

#endif