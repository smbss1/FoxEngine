//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_CONTENTBROWSERPANEL_HPP
#define FOXENGINE_CONTENTBROWSERPANEL_HPP

#include "Renderer/Texture.hpp"
#include "Panel.hpp"
#include <unordered_map>
#include "Core/Base.hpp"
#include "ThumbnailCache.hpp"

namespace fox
{
    struct OnProjectChangeEvent;
    struct OnContextChangeEvent;
    class Scene;
    class ContentBrowserPanel : public Panel
    {
    public:
        ContentBrowserPanel();

        void OnImGui(bool& isOpen) override;
        void OnProjectChange(const OnProjectChangeEvent& event);
        void OnContextChange(const OnContextChangeEvent& event);

    private:
		fs::path m_BaseDirectory;
		fs::path m_CurrentDirectory;
        Ref<Scene> m_Context;

        Ref<Texture2D> m_pFolderIcon = nullptr;
        Ref<Texture2D> m_pFileIcon = nullptr;


		void RefreshAssetTree();

		//Ref<Project> m_Project;
		Ref<ThumbnailCache> m_ThumbnailCache;

		struct TreeNode
		{
			fs::path Path;
			AssetHandle Handle = 0;

			uint32_t Parent = (uint32_t)-1;
			std::map<fs::path, uint32_t> Children;

			TreeNode(const fs::path& path, AssetHandle handle)
				: Path(path), Handle(handle) {}
		};

		std::vector<TreeNode> m_TreeNodes;

		std::map<fs::path, std::vector<fs::path>> m_AssetTree;

		enum class Mode
		{
			Asset = 0, FileSystem = 1
		};

		Mode m_Mode = Mode::Asset;
    };
}

#endif //FOXENGINE_CONTENTBROWSERPANEL_HPP
