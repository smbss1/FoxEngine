//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_CONTENTBROWSERPANEL_HPP
#define FOXENGINE_CONTENTBROWSERPANEL_HPP

#include "Renderer/Texture.hpp"
#include "Panel.hpp"
#include <unordered_map>
#include <filesystem>

namespace fox
{
    class ContentBrowserPanel : public Panel
    {
    public:
        ContentBrowserPanel();

        void OnImGui() override;
        void OnProjectOpen();

    private:
        std::filesystem::path m_oCurrentDirectory;

        ref<Texture2D> m_pFolderIcon = nullptr;
        ref<Texture2D> m_pFileIcon = nullptr;
        std::unordered_map<std::string, ref<Texture2D>> m_ImageIcons;
    };
}

#endif //FOXENGINE_CONTENTBROWSERPANEL_HPP
