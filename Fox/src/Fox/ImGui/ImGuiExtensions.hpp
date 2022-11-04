//
// Created by samuel on 24/10/22.
//

#ifndef FOXENGINE_IMGUIEXTENSIONS_HPP
#define FOXENGINE_IMGUIEXTENSIONS_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>
#include "imgui.h"
#include "glm/detail/setup.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Asset/Asset.hpp"

namespace fox::UI
{
    const char* GenerateID();

    void ShiftCursorX(float distance);
    void ShiftCursorY(float distance);
    void ShiftCursor(float x, float y);

    void PushID();
    void PopID();
    void BeginProperties(ImGuiTableFlags flags = ImGuiTableFlags_Resizable);
    void EndProperties();
    void BeginPropertyGrid(const char* label, const char* tooltip, bool rightAlignNextColumn = false);
    void EndPropertyGrid();
    bool Checkbox(const std::string& label, bool& value);

    bool Slider(const std::string& label, float& value, float min, float max, ImGuiSliderFlags flags = 0);
    bool Slider(const std::string& label, double& value, double min, double max, ImGuiSliderFlags flags = 0);
    bool Slider(const std::string& label, uint32_t& value, uint32_t min, uint32_t max, ImGuiSliderFlags flags = 0);
    bool Slider(const std::string& label, uint64_t& value, uint64_t min, uint64_t max, ImGuiSliderFlags flags = 0);
    bool Slider(const std::string& label, int32_t& value, int32_t min, int32_t max, ImGuiSliderFlags flags = 0);
    bool Slider(const std::string& label, int64_t& value, int64_t min, int64_t max, ImGuiSliderFlags flags = 0);

    bool DragScalar(const std::string& label, uint32_t& value, float speed = 1.0f, uint32_t min = 0.0f, uint32_t max = 0.0f);
    bool DragScalar(const std::string& label, int32_t &value, float speed = 1.0f, int32_t min = 0.0f, int32_t max = 0.0f);
    bool DragScalar(const std::string& label, uint64_t &value, float speed = 1.0f, uint64_t min = 0.0f, uint64_t max = 0.0f);
    bool DragScalar(const std::string& label, int64_t &value, float speed = 1.0f, int64_t min = 0.0f, int64_t max = 0.0f);
    bool DragScalar(const std::string& label, float& value, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
    bool DragScalar(const std::string& label, double &value, float speed = 1.0f, double min = 0.0f, double max = 0.0f);
    bool DragScalarN(const std::string& label, uint32_t *value, int components, float speed = 1.0f, uint32_t min = 0.0f, uint32_t max = 0.0f);
    bool DragScalarN(const std::string& label, int32_t *value, int components, float speed = 1.0f, int32_t min = 0.0f, int32_t max = 0.0f);
    bool DragScalarN(const std::string& label, uint64_t *value, int components, float speed = 1.0f, uint64_t min = 0.0f, uint64_t max = 0.0f);
    bool DragScalarN(const std::string& label, int64_t *value, int components, float speed = 1.0f, int64_t min = 0.0f, int64_t max = 0.0f);
    bool DragScalarN(const std::string& label, float *value, int components, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
    bool DragScalarN(const std::string& label, double *value, int components, float speed = 1.0f, double min = 0.0f, double max = 0.0f);

    template<glm::length_t L, typename T>
    bool DragScalar(const std::string& label, glm::vec<L, T>& value, float speed = 1.0f, T min = 0.0f, T max = 0.0f) {
        return DragScalarN(label, glm::value_ptr(value), L, speed, min, max);
    }

    bool DragScalar(const std::string& label, glm::vec3& value, float speed = 1.0f, float min = 0.0f, float max = 0.0f);

    template<typename T>
    bool ColorEdit(const std::string& label, glm::vec<3, T>& value)
    {
        return ImGui::ColorEdit3(label.c_str(), glm::value_ptr(value));
    }

    template<typename T>
    bool ColorEdit(const std::string& label, glm::vec<4, T>& value)
    {
        return ImGui::ColorEdit4(label.c_str(), glm::value_ptr(value));
    }

    bool AssetField(const std::string& label, AssetHandle& value, bool handlePayload = false, const std::vector<std::string>& extensions = {});
    void PreviewTexture(AssetHandle& value, const std::vector<std::string>& extensions);
    bool ImageButton(AssetHandle& handle);
    bool HandleContentBrowserPayload(AssetHandle& handle, const std::vector<std::string>& extensions);
    bool HandleContentBrowserPayloadCustom(const std::vector<std::string>& extensions, std::function<void(std::filesystem::path&)> func);
    bool DropBoxAsset(AssetHandle& handle, const std::vector<std::string>& extensions);
    bool Enum(const std::string& name, const char* typeString[], int size, int index, int& valueFrom);
    bool ToggleButton(const std::string& str_id, bool& v);

    bool Property(const std::string& label, bool& value, const char* helpText = "");
//    void Label(const char* label);

    bool IsItemDisabled();
    void DrawItemActivityOutline(float rounding = 0.0f, bool drawWhenInactive = false, ImColor colourWhenActive = ImColor(80, 80, 80));

    bool Property(const std::string& name, const char* typeString[], int size, int index, int& valueFrom);
    bool Property(const std::string& label, bool& value, const char* helpText);
    bool PropertyInput(const std::string& label, char* value, size_t size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
    bool InputText(const std::string& label, char* value, size_t size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

    template<glm::length_t L, typename T>
    bool PropertyColor(const std::string& label, glm::vec<L, T>& value)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = ColorEdit(GenerateID(), value);
        UI::EndPropertyGrid();
        return result;
    }

    template<typename T>
    bool PropertySlider(const std::string& label, T& value, T min, T max, ImGuiSliderFlags flags)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = Slider(GenerateID(), value, min, max, flags);
        UI::EndPropertyGrid();
        return result;
    }

    template<typename T>
    bool Property(const std::string& label, T& value, float speed = 1.0f, T min = 0.0f, T max = 0.0f)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = DragScalar(GenerateID(), value, speed, min, max);
        UI::EndPropertyGrid();
        return result;
    }

    template<glm::length_t L, typename T>
    bool Property(const std::string& label, glm::vec<L, T>& value, float speed = 1.0f, T min = 0.0f, T max = 0.0f)
    {
        UI::BeginPropertyGrid(label.c_str(), nullptr);
        bool result = DragScalar(GenerateID(), value, speed, min, max);
        UI::EndPropertyGrid();
        return result;
    }
}


#endif //FOXENGINE_IMGUIEXTENSIONS_HPP
