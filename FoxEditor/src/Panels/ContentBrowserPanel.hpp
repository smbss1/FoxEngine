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
        std::filesystem::path m_oCurrentDirectory;
        Ref<Scene> m_Context;

        Ref<Texture2D> m_pFolderIcon = nullptr;
        Ref<Texture2D> m_pFileIcon = nullptr;
        std::unordered_map<std::string, Ref<Texture2D>> m_ImageIcons;
    };
}

#endif //FOXENGINE_CONTENTBROWSERPANEL_HPP
