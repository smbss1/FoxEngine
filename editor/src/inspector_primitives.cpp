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
#include <Core/Logger/Logger.hpp>
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
    auto ConvertToDegree = get_metadata("CONVERT_TO_DEGREE");
    if (ConvertToDegree)
        data = glm::degrees(data);

    changed |= fox::imgui::DragVec3(data);

    if (ConvertToDegree)
        data = glm::radians(data);

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