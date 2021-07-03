//
// Created by samuel on 02/07/2021.
//

#ifndef FOXENGINE_CONTENTBROWSERPANEL_HPP
#define FOXENGINE_CONTENTBROWSERPANEL_HPP

#include <filesystem>

namespace fox
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void OnImGui();
        void OnProjectOpen();

    private:
        std::filesystem::path m_oCurrentDirectory;
    };
}

#endif //FOXENGINE_CONTENTBROWSERPANEL_HPP
