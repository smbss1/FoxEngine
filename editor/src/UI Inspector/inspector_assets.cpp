//
// Created by samuel on 13/07/2021.
//

#include <ImGuiExtension.hpp>
#include <imgui.h>
#include <FileSystem/Path.hpp>
#include <imgui_internal.h>
#include <filesystem>
#include <FileSystem/SupportedFileFormat.hpp>
#include "Inspector/inspector_assets.hpp"

bool inspector_texture::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<fox::ref<fox::Texture2D>>();
    bool changed = false;
    float available = std::min(64.0f, ImGui::GetContentRegionAvailWidth() / 1.5f);

    ImVec2 size = {available, available};
    if(data)
    {
        fox::imgui::ImageWithAspect(data, ImVec2(data->GetWidth(), data->GetHeight()), size);
    }
    else
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return false;
        ImRect bb(window->DC.CursorPos,
                  ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
        ImGui::ItemSize(bb);
        if(!ImGui::ItemAdd(bb, 0))
            return false;

        fox::imgui::RenderFrameEx(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true, 0.0f, 1.0f);
    }

    // Drag and Drop Texture from Content Browser
    if (ImGui::BeginDragDropTarget())
    {
        if(ImGui::IsDragDropPayloadBeingAccepted())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);

        // Find if the extension is supported
        for(const auto& type : fox::ex::GetSupportedFormats<fox::Texture2D>())
        {
            auto payload = ImGui::AcceptDragDropPayload(type.c_str());
            if(payload)
            {
                // This extension is supported, so load the texture

                std::string strAbsPath(reinterpret_cast<const char*>(payload->Data),
                                          std::size_t(payload->DataSize));
                data = fox::Texture2D::Create(strAbsPath);
                var = data;
                changed = true; // Notifiy that the property changed
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Add a Clear button, to remove the texture if not needed
    ImGui::SameLine();
    if(ImGui::Button("CLEAR"))
    {
        data = nullptr;
        var = data;
        changed = true;
    }

    return changed;
}

//INSPECTOR_TRACK_FUNCTION(inspector_texture, fox::Texture2D)

bool inspector_texture::inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    return false;
}