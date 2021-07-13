//
// Created by samuel on 13/07/2021.
//

#include <ImGuiExtension.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Inspector/inspector_primitives.hpp"

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