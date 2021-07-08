//
// Created by samuel on 25/06/2021.
//

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <Core/Application.hpp>
#include <Components/EntityName.hpp>
#include <Components/Transform.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/SpriteRenderer.hpp>
#include <ImGui/imgui_internal.h>
#include <Utils/Path.hpp>
#include "SceneHierarchyPanel.hpp"

//const float toolbarSize = 10;
//float menuBarHeight = 100;
//
//void DockSpaceUI()
//{
//    ImGuiViewport* viewport = ImGui::GetMainViewport();
//    ImVec2 pos = {viewport->Pos.x, viewport->Pos.y + toolbarSize};
//    ImVec2 size = {viewport->Size.x, viewport->Size.y - toolbarSize};
//    ImGui::SetNextWindowPos(pos);
//    ImGui::SetNextWindowSize(size);
//    ImGui::SetNextWindowViewport(viewport->ID);
//    ImGuiWindowFlags window_flags = 0
//                                    | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
//                                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
//                                    | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
//                                    | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//    ImGui::Begin("Master DockSpace", NULL, window_flags);
//    ImGuiID dockMain = ImGui::GetID("MyDockspace");
//
//    // Save off menu bar height for later.
//    menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();
//
//    ImGui::DockSpace(dockMain);
//    ImGui::End();
//    ImGui::PopStyleVar(3);
//}
//
//void ToolbarUI()
//{
//    ImGuiViewport* viewport = ImGui::GetMainViewport();
//    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
//    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
//    ImGui::SetNextWindowViewport(viewport->ID);
//
//    ImGuiWindowFlags window_flags = 0
//                                    | ImGuiWindowFlags_NoDocking
//                                    | ImGuiWindowFlags_NoTitleBar
//                                    | ImGuiWindowFlags_NoResize
//                                    | ImGuiWindowFlags_NoMove
//                                    | ImGuiWindowFlags_NoScrollbar
//                                    | ImGuiWindowFlags_NoSavedSettings
//    ;
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
//    ImGui::Begin("TOOLBAR", NULL, window_flags);
//    ImGui::PopStyleVar();
//
//    ImGui::Button("Toolbar goes here", ImVec2(0, 37));
//
//    ImGui::End();
//}
//
//void UndockWidget(ImVec2 icon_size, ImGuiAxis axis)
//{
//    ImGuiWindow* window = ImGui::GetCurrentWindow();
//
//    ImVec2 size = icon_size;
//    size[axis] = ImFloor(size[axis] * 0.30f);
//    ImVec2 p = ImGui::GetCursorScreenPos();
//    ImGui::InvisibleButton("##Undock", size);
//
//    const bool is_hovered = ImGui::IsItemHovered();
//    const bool is_active = ImGui::IsItemActive();
//    if (is_active)
//        ImGui::StartMouseMovingWindowOrNode(window, window->DockNode, true);
//
//    const ImU32 col = ImGui::GetColorU32(is_active ? ImGuiCol_ButtonActive : is_hovered ? ImGuiCol_ButtonHovered : ImGuiCol_TextDisabled, 0.4f);
//    window->DrawList->AddRectFilled(p, {p.x + size.x, p.y + size.y}, col);
//}

namespace fox
{
    SceneHierarchyPanel::SceneHierarchyPanel() : m_pScriptLib(new_scope<SharedLib>())
    {
    }

    SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) : SceneHierarchyPanel()
    {
        SetContext(context);
    }

    SceneHierarchyPanel::~SceneHierarchyPanel()
    {
        m_pContext.reset();
        m_pScriptLib.reset();
    }

    void SceneHierarchyPanel::SetContext(const ref<Scene>& context)
    {
        m_pContext = context;
        m_SelectedEntity = {};

        if (!m_pScriptLib->GetHandle()) {

            std::unordered_map<size_t, std::string> (*GetScriptsNames)();
            std::unordered_map<size_t, ScriptCreator> (*GetScripts)();
            fox::info("Open the native script library");

            try {
                m_pScriptLib->open("../Project/libfox_native_script" DL_EXT);
                m_pScriptLib->sym("GetScriptsNames", GetScriptsNames);
                m_pScriptLib->sym("GetScripts", GetScripts);
                m_vScriptsNames = GetScriptsNames();
                m_vScripts = GetScripts();
                context->GetApp().SetScriptsArray(m_vScripts);
                for (auto name : m_vScriptsNames) {
                    fox::info("Script: '%'", name.second);
                }
            }
            catch (std::exception &e) {
                fox::error("%", e.what());
            }
        }
    }

    void SceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin("Scene Hierarchy");
        m_pContext->GetWorld().each([&](auto entityid)
         {
            fox::Entity entity {&m_pContext->GetWorld(), entityid};
             DrawEntityNode(entity);
         });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectedEntity = {};

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_pContext->NewEntity();

            ImGui::EndPopup();
        }
        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectedEntity)
        {
            DrawComponents(m_SelectedEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity &entity)
    {
        auto &name = entity.get<EntityName>().value();
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool expanded = ImGui::TreeNodeEx((void*)entity.get_id(), flags, "%s", name.name.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        bool bIsDeleted = false; // Is the entity deleted ?
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
                bIsDeleted = true;

            ImGui::EndPopup();
        }

        if (expanded)
        {
//            // Draw tree child node
//            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
//            bool expanded = ImGui::TreeNodeEx((void*)5446, flags, "%s", name.name.c_str());
//            if (expanded)
//                ImGui::TreePop();
            ImGui::TreePop();
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("SceneHierarchy", &entity, sizeof(Entity));
            ImGui::Text(name.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchy");
            if (payload != nullptr) {
                Entity *e = static_cast<Entity *>(payload->Data);
                fox::info("Entity name: %", e->get<EntityName>()->name);
            }
            ImGui::EndDragDropTarget();
        }

        if (bIsDeleted) {
            entity.destroy();
            if (m_SelectedEntity == entity)
                m_SelectedEntity = {};
        }
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
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
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    void SceneHierarchyPanel::DrawComponents(Entity& entity)
    {
        auto has_name = entity.get<EntityName>();
        if (has_name)
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, has_name->name.c_str());

            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                has_name->name = buffer;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem("Camera"))
            {
                m_pContext->AddComponent<CameraComponent>(m_SelectedEntity);
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Sprite Renderer"))
            {
                m_pContext->AddComponent<SpriteRenderer>(m_SelectedEntity);
                ImGui::CloseCurrentPopup();
            }

            for (auto script : m_vScriptsNames)
            {
                const std::string& name = script.second;
                const std::size_t& tag = script.first;
                if (ImGui::MenuItem(name.c_str()))
                {
                    auto has_native_script = m_pContext->GetWorld().get_component<NativeScript>(m_SelectedEntity);
                    if (!has_native_script)
                        m_pContext->AddComponent<NativeScript>(m_SelectedEntity);
                    has_native_script = m_pContext->GetWorld().get_component<NativeScript>(m_SelectedEntity);
                    has_native_script->add(tag, std::move(m_vScripts[tag]()));
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();

        DrawComponent<fox::TransformComponent>("Transform", entity, [](fox::TransformComponent& transform)
        {
            DrawVec3Control("Position", transform.position);
            glm::vec3 rot = glm::degrees(transform.rotation);
            DrawVec3Control("Rotation", transform.rotation);
            transform.rotation = glm::radians(rot);
            DrawVec3Control("Scale", transform.scale, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComp)
        {
            auto& camera = cameraComp.camera;

            ImGui::Checkbox("Primary", &cameraComp.Primary);

            const char* projectionTypeString[] = { "Perspective", "Orthographic" };
            const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < 2; ++i)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeString[i];
                    if (ImGui::Selectable(projectionTypeString[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeString[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float size = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &size))
                    camera.SetOrthographicSize(size);

                float near = camera.GetOrthographicNearClip();
                if (ImGui::DragFloat("Near", &near))
                    camera.SetOrthographicNearClip(near);

                float far = camera.GetOrthographicFarClip();
                if (ImGui::DragFloat("Far", &far))
                    camera.SetOrthographicFarClip(far);

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);
            }
            else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Size", &fov))
                    camera.SetPerspectiveVerticalFOV(glm::radians(fov));

                float near = camera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &near))
                    camera.SetPerspectiveNearClip(near);

                float far = camera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &far))
                    camera.SetPerspectiveFarClip(far);
            }
        });

        DrawComponent<SpriteRenderer>("Sprite Renderer", entity,[](SpriteRenderer& sprite)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));

            if (sprite.m_pSprite)
            {
                Path path(sprite.m_strFilepath);
                auto filename = path.filename();
                ImGui::InputText("##SpriteRendererTexture", (char*)filename.c_str(), filename.size(), ImGuiInputTextFlags_ReadOnly);
            }
            else
            {
                ImGui::InputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_HANDLE_ID"))
                {
                    // TODO: Find a way to check the correct data we receive
//                    IM_ASSERT(payload->DataSize == sizeof(std::string));
                    std::string texturePath = (char *)payload->Data;
                    sprite.m_pSprite = Texture2D::Create(texturePath);
                    sprite.m_strFilepath = texturePath;
                }
                ImGui::EndDragDropTarget();
            }
        });

        DrawScripts(entity, [](ScriptableBehaviour& script)
        {
        });
    }

    template<typename T>
    void SceneHierarchyPanel::DrawComponent(const std::string &name, Entity entity,  std::function<void(T &)> callback)
    {
        const ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
        auto has_component = entity.get<T>();

        if (has_component)
        {
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeFlags, name.c_str());
            ImGui::PopStyleVar();

            ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
            if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool bIsDeleted = false; // Is the component removed ?
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    bIsDeleted = true;
                ImGui::EndPopup();
            }

            if (open)
            {
                callback(*has_component);
                ImGui::TreePop();
            }

            if (bIsDeleted)
            {
                entity.template remove<T>();
            }
        }
    }

    void SceneHierarchyPanel::DrawScripts(Entity entity, std::function<void(ScriptableBehaviour&)> callback)
    {
        const ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                             ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
        auto has_component = entity.get<NativeScript>();

        if (has_component)
        {
            for (auto& script : has_component->m_vScripts)
            {
                ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4});
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::Separator();
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeFlags, m_vScriptsNames[script.first].c_str());
                ImGui::PopStyleVar();

                ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
                if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }

                bool bIsDeleted = false; // Is the component removed ?
                if (ImGui::BeginPopup("ComponentSettings"))
                {
                    if (ImGui::MenuItem("Remove Component"))
                        bIsDeleted = true;
                    ImGui::EndPopup();
                }

                if (open)
                {
                    callback(*script.second);
                    ImGui::TreePop();
                }

                if (bIsDeleted)
                {
                    has_component->remove(script.first);
                }
            }
        }
    }
}