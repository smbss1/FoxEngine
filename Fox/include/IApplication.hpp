
// #ifndef FOX_IAPP_HPP_
// #define FOX_IAPP_HPP_

// #include "SceneManager.hpp"
// #include "ResourceManager.hpp"

// namespace fox
// {
//     class IApplication
//     {
//     public:
//         virtual ~IApplication() = 0;
    
//         virtual void run() = 0;

//         // -------- Scene Manager ----------------------------------
//         virtual SceneId add_scene(ref<Scene> pScene) = 0;
//         virtual void switch_scene(SceneId id) = 0;
//         virtual void remove_scene(SceneId id) = 0;
//         virtual ref<Scene> get_active() = 0;
//         // ---------------------------------------------------------------------

//         // -------- Resource Manager ----------------------------------
//         template <typename T>
// 		virtual bool add_manager(IAssetManager* pAssetManager) noexcept = 0;
// 		template <typename T>
// 		virtual bool add_asset(const std::string& strAssetName, const std::string& strAssetPath) noexcept = 0;
// 		template<typename TAsset>
// 		virtual TAsset* get_asset(const std::string& strAssetName) = 0;
// 		template<typename TAsset>
// 		virtual bool remove_asset(const std::string& strAssetName) noexcept = 0;
//         virtual ResourceManager& get_resource_manager() const = 0;
//         // ---------------------------------------------------------------------
//     };
// }

// #endif