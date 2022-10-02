// //
// // Created by samuel on 13/07/2021.
// //

// #include <imgui.h>
// #include <imgui_internal.h>
// #include "Inspector/inspector.hpp"

// void Tooltip(const rttr::property& prop)
// {
//     auto& g = *ImGui::GetCurrentContext();
//     if(!g.DragDropActive && ImGui::IsItemHovered())
//     {
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//         auto tooltip = prop.get_metadata("tooltip");
//         if(tooltip)
//         {
//             ImGui::BeginTooltip();
//             ImGui::TextUnformatted(tooltip.to_string().c_str());
//             ImGui::EndTooltip();
//         }
//     }
// }

// void Tooltip(const std::string& tooltip)
// {
//     auto& g = *ImGui::GetCurrentContext();
//     if(!g.DragDropActive && ImGui::IsItemHovered())
//     {
//         ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//         ImGui::BeginTooltip();
//         ImGui::TextUnformatted(tooltip.c_str());
//         ImGui::EndTooltip();
//     }
// }

// property_layout::property_layout(const rttr::property& prop, bool columns /*= true*/, float columnWidth /*=100*/)
// {
//     std::string pretty_name = prop.get_name().to_string();
//     auto meta_pretty_name = prop.get_metadata("pretty_name");
//     if(meta_pretty_name)
//     {
//         pretty_name = meta_pretty_name.get_value<std::string>();
//     }

//     if(columns)
//     {
//         if(ImGui::GetColumnsCount() > 1)
//             ImGui::EndColumns();
//         ImGui::BeginColumns("properties", 2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
//         ImGui::SetColumnWidth(0, columnWidth);
//     }

//     ImGui::PushID(pretty_name.c_str());
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextUnformatted(pretty_name.c_str());
//     Tooltip(prop);
//     ImGui::NextColumn();
// }

// property_layout::property_layout(const std::string& name, bool columns /*= true*/, float columnWidth /*=100*/)
// {
//     if(columns)
//     {
//         if(ImGui::GetColumnsCount() > 1)
//             ImGui::EndColumns();
//         ImGui::BeginColumns("properties", 2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
//         ImGui::SetColumnWidth(0, columnWidth);
//     }

//     ImGui::PushID(name.c_str());
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextUnformatted(name.c_str());
//     ImGui::NextColumn();
// }

// property_layout::property_layout(const std::string& name, const std::string& tooltip, bool columns /*= true*/, float columnWidth /*=100*/)
// {
//     if(columns)
//     {
//         if(ImGui::GetColumnsCount() > 1)
//             ImGui::EndColumns();
//         ImGui::BeginColumns("properties", 2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
//         ImGui::SetColumnWidth(0, columnWidth);
//     }

//     ImGui::PushID(name.c_str());
//     ImGui::AlignTextToFramePadding();
//     ImGui::TextUnformatted(name.c_str());
//     Tooltip(tooltip);
//     ImGui::NextColumn();
// }

// property_layout::~property_layout()
// {
//     if(ImGui::GetColumnsCount() > 1)
//         ImGui::EndColumns();
//     ImGui::PopID();
// }