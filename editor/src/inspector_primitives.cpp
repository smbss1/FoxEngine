//
// Created by samuel on 13/07/2021.
//

#include <ImGuiExtension.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <Utils/Path.hpp>
#include <imgui_internal.h>
#include <filesystem>
#include <SupportedFileFormat.hpp>
#include "Inspector/inspector_primitives.hpp"

bool inspector_vec2::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<glm::vec2>();
    bool changed = false;

    changed |= ImGui::DragFloat2("##Color", glm::value_ptr(data));
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_vec3::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<glm::vec3>();
    bool changed = false;
//    const char* names[] = {"X", "Y"};

//    std::uint32_t min = 0;
//    std::uint32_t max = 0;
//
//    auto min_var = get_metadata("min");
//    if(min_var)
//        min = min_var.to_uint32();
//    auto max_var = get_metadata("max");
//    if(max_var)
//        max = max_var.to_uint32();

    changed |= fox::imgui::DragVec3(data);
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_vec4::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<glm::vec4>();
    bool changed = false;

    changed |= ImGui::ColorEdit4("##Color", glm::value_ptr(data));
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_texture::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
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