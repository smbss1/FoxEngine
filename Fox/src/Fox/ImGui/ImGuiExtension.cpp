//
// Created by samuel on 13/07/2021.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiExtension.hpp"

namespace fox
{
    namespace imgui
    {
        static void PushMultiItemsWidthsAndLabels(const char* labels[], int components, float w_full = 0.0f)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImGuiStyle& style = GImGui->Style;
            if(w_full <= 0.0f)
                w_full = ImGui::GetContentRegionAvailWidth();

            const float w_item_one =
                    ImMax(1.0f, (w_full - (style.ItemInnerSpacing.x * 2.0f) * (components - 1)) / (float)components) -
                    style.ItemInnerSpacing.x;
            for(int i = 0; i < components; i++)
                window->DC.ItemWidthStack.push_back(w_item_one - ImGui::CalcTextSize(labels[i]).x);
            window->DC.ItemWidth = window->DC.ItemWidthStack.back();
        }

        bool DragVec3(glm::vec3& values, float resetValue, float columnWidth)
        {
            bool value_changed;
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];
            const char* labels[] = {"X", "Y", "Z"};

//            ImGui::PushID(label.c_str());
//
//            ImGui::Columns(2);
//            ImGui::SetColumnWidth(0, columnWidth);
//            ImGui::Text(label.c_str());
//            ImGui::NextColumn();

//            PushMultiItemsWidthsAndLabels(labels, 3);
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0,0});

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            // X
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize)) {
                values.x = resetValue;
                value_changed = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            value_changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Y
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize)) {
                values.y = resetValue;
                value_changed = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            value_changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Z
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Z", buttonSize)) {
                values.z = resetValue;
                value_changed = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            value_changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::Columns(1);

//            ImGui::Columns(1);
//            ImGui::PopID();
            return value_changed;
        }

        bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
        {
            bool result = false;
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text(label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0,0});

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            // X
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
            {
                values.x = resetValue;
                result = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if(ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
                result = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Y
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
            {
                values.y = resetValue;
                result = true;
            }

            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
                result = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Z
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Z", buttonSize))
            {
                values.z = resetValue;
                result = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
                result = true;

            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();

            return result;
        }

        void ImageWithAspect(Ref<Texture2D> texture, ImVec2 texture_size, ImVec2 size, const ImVec2& _uv0,
                             const ImVec2& _uv1, const ImVec4& tint_col, const ImVec4& border_col)
        {
            ImVec2 uv0 = _uv0;
            ImVec2 uv1 = _uv1;
            bool bIsOriginBl = true;

            if(texture)
            {
                if(bIsOriginBl)
                {
                    uv0 = {0.0f, 1.0f};
                    uv1 = {1.0f, 0.0f};
                }
            }


            float w = texture_size.x;
            float h = texture_size.y;
            float max_size = ImMax(size.x, size.y);
            float aspect = w / h;
            if(w > h)
            {
                float m = ImMin(max_size, w);

                size.x = m;
                size.y = m / aspect;
            }
            else if(h > w)
            {
                float m = ImMin(max_size, h);

                size.x = m * aspect;
                size.y = m;
            }


            // Render the image
            auto pos = ImGui::GetCursorScreenPos();
            ImGuiWindow* window = ImGui::GetCurrentWindow();

            ImRect bb(window->DC.CursorPos,
                      ImVec2(window->DC.CursorPos.x + max_size, window->DC.CursorPos.y + max_size));
            ImGui::ItemSize(bb);
            ImGui::ItemAdd(bb, 0);

            auto pos2 = ImGui::GetCursorScreenPos();

            if(size.x > size.y)
                pos.y += (max_size - size.y) * 0.5f;
            if(size.x < size.y)
                pos.x += (max_size - size.x) * 0.5f;

            ImGui::SetCursorScreenPos(pos);

            ImGui::Image((ImTextureID)texture->GetRendererID(), size, uv0, uv1, tint_col, border_col);

            ImGui::SetCursorScreenPos(pos2);
        }

        void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();

            if(border)
            {
                window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1),
                                          ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, 15, thickness);
                window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, 15, thickness);
            }
        }
    }
}
