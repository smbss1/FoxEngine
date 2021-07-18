//
// Created by samuel on 13/07/2021.
//

#include <imgui.h>
#include <filesystem>
#include <Logger/Logger.hpp>
#include <Components/Animator/Track.hpp>

#include "Inspector/inspector_primitives.hpp"

bool inspector_int::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<int>();
    bool changed = false;

    changed |= ImGui::DragInt("##Value", &data);
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_float::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<float>();
    bool changed = false;

    changed |= ImGui::DragFloat("##Value", &data);
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_stdstring::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<std::string>();
    bool changed = false;

    ImGui::Text("%s", data.c_str());
    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_string::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<char*>();
    bool changed = false;

    ImGui::Text("%s", data);

    if(changed)
    {
        var = data;
        return true;
    }
    return false;
}

bool inspector_bool::inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
    auto data = var.get_value<bool>();
    bool changed = false;

    changed |= ImGui::Checkbox("##Value", &data);

    if(changed)
        var = data;
    return changed;
}

INSPECTOR_TRACK_FUNCTION(inspector_int, int)
INSPECTOR_TRACK_FUNCTION(inspector_float, float)
INSPECTOR_TRACK_FUNCTION(inspector_bool, bool)

bool inspector_stdstring::inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata) {
//        Track<inspector_stdstring>* new_track = new Track<inspector_stdstring>();
//        var = new_track;
    return false;
}

bool inspector_string::inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata) {
//        Track<inspector_stdstring>* new_track = new Track<inspector_stdstring>();
//        var = new_track;
    return false;
}

//INSPECTOR_TRACK_FUNCTION(inspector_stdstring, std::string)
//INSPECTOR_TRACK_FUNCTION(inspector_string, char*)