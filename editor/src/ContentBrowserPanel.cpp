//
// Created by samuel on 02/07/2021.
//

#include <ImGui/imgui.h>
#include <FPaths.hpp>
#include "ContentBrowserPanel.hpp"

namespace fox
{
    ContentBrowserPanel::ContentBrowserPanel()
        : m_oCurrentDirectory(FPaths::AssetsDir())
    {
    }

    void ContentBrowserPanel::OnImGui()
    {
        ImGui::Begin("Content Browser");

        if (!m_oCurrentDirectory.empty()) {

            if (m_oCurrentDirectory != std::filesystem::path(FPaths::AssetsDir())) {
                if (ImGui::Button("<")) {
                    m_oCurrentDirectory = m_oCurrentDirectory.parent_path();
                }
            }

            for (auto &directoryEntry : std::filesystem::directory_iterator(m_oCurrentDirectory)) {
                const auto &path = directoryEntry.path();
                auto relativePath = std::filesystem::relative(path, FPaths::AssetsDir());
                auto strFilenamePath = relativePath.filename().string();

                if (directoryEntry.is_directory()) {
                    if (ImGui::Button(strFilenamePath.c_str())) {
                        m_oCurrentDirectory /= path.filename();
                    }
                } else {
                    if (ImGui::Button(strFilenamePath.c_str())) {
                    }
                }
            }
        }

        ImGui::End();

    }

    void ContentBrowserPanel::OnProjectOpen()
    {
        m_oCurrentDirectory = FPaths::AssetsDir();
    }
}