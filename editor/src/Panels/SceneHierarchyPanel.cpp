//
// Created by samuel on 25/06/2021.
//

#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "imgui_internal.h"
#include "Utils/Path.hpp"
#include "Save for later/FPaths.hpp"
#include "Events/EventSystem.hpp"
#include "../EditorEvent.hpp"
#include <filesystem>
#include "glm/gtc/type_ptr.hpp"

#include "SceneHierarchyPanel.hpp"
#include "ImGui/ImGuiExtension.hpp"
#include "Scripting/ScriptEngine.hpp"

#include "Reflection/Core/Reflect.hpp"
#include "ImGui/ImGuiFieldDrawer.hpp"
#include "Core/EditorLogger.hpp"

namespace fox
{
    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel()
    {
    }

    SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) : SceneHierarchyPanel()
    {
        SetContext(context);
    }

    SceneHierarchyPanel::~SceneHierarchyPanel()
    {
        m_pContext.reset();
    }

    void SceneHierarchyPanel::SetContext(const ref<Scene>& context)
    {
        m_pContext = context;
        m_SelectedEntity = {};

        event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
    }

    void SceneHierarchyPanel::OnImGui()
    {
        DrawLog("title");

        ImGui::Begin("Scene Hierarchy");
        {
            if (m_pContext)
            {
                m_pContext->m_Registry.each([&](auto entityID)
                {
                    Entity entity {entityID, m_pContext.get()};
                    DrawEntityNode(entity);
                });

                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
                    m_SelectedEntity = {};
                    event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
                }

                // Right-click on blank space
                if (ImGui::BeginPopupContextWindow(0, 1, false)) {
                    if (ImGui::MenuItem("Create Empty Entity"))
                        m_pContext->NewEntity();

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity) {
                DrawComponents(m_SelectedEntity);
            }
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity &entity)
    {
        auto &name = entity.get<EntityName>();
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool expanded = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetUUID(), flags, "%s", name.name.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
            event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
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
           // Draw tree child node
        //    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        //    bool xpanded = ImGui::TreeNodeEx((void*)5446, flags, "%s", name.name.c_str());
        //    if (xpanded)
        //        ImGui::TreePop();
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
                fox::info("Entity name: %", e->get<EntityName>().name);
            }
            ImGui::EndDragDropTarget();
        }

        if (bIsDeleted) {
            m_pContext->DestroyEntity(m_SelectedEntity);
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = {};
                event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
            }
        }
    }

    template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (!entity.has<T>())
            return;
        auto& component = entity.get<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (removeComponent)
            entity.remove<T>();
	}

    template<typename T>
    static void DrawComponent(const std::string& name, Entity entity)
    {
        DrawComponent<T>(name, entity, [](auto& component) {
            auto reflect = Reflect::Resolve<T>();
            auto members = reflect->GetDataMembers();
            for (Reflect::DataMember* member : members)
            {
                auto value = member->Get(component);
                const Reflect::TypeDescriptor* type = member->GetType();

//                        fox::info("Name: %", member->GetName());
//                        fox::info("Type: %", type->GetName());
//                        fox::info("Value: %", value.Get());

                if (ImGuiFieldDrawer::Draw(type, member->GetName(), value))
                {
                    member->Set(component, value);
                }
            }
        });
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.has<EntityName>())
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, entity.get<EntityName>().name.c_str());

            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                entity.get<EntityName>().name = buffer;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<ScriptComponent>("Script Component");
            DisplayAddComponentEntry<SpriteRenderer>("Sprite Renderer");
            DisplayAddComponentEntry<Rigidbody2D>("Rigidbody 2D");
            DisplayAddComponentEntry<BoxCollider2D>("BoxCollider 2D");
            DisplayAddComponentEntry<CircleRenderer>("Circle Renderer");
            DisplayAddComponentEntry<CircleCollider2D>("Circle Collider");

            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity);
        DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComp)
       {
           auto& camera = cameraComp.camera;

           ImGui::Checkbox("Primary", &cameraComp.Primary);

           const char* projectionTypeString[] = { "Perspective", "Orthographic" };

           int result = -1;
           if (ImGuiFieldDrawer::DrawEnum("Projection", projectionTypeString, 2, (int)camera.GetProjectionType(), result))
           {
               camera.SetProjectionType((SceneCamera::ProjectionType)result);
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

        DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_pContext](auto& component) mutable
        {
            bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

            static char buffer[64];
            strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));

            if (!scriptClassExists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

            if (ImGui::InputText("Class", buffer, sizeof(buffer)))
                component.ClassName = buffer;

            // Fields
            bool sceneRunning = scene->IsRunning();
            if (sceneRunning)
            {
                ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
                if (scriptInstance)
                {
                    const auto& fields = scriptInstance->GetScriptClass()->GetFields();
                    for (const auto& [name, field] : fields)
                    {
                        if (field.Type == ScriptFieldType::Float)
                        {
                            float data = scriptInstance->GetFieldValue<float>(name);
                            if (ImGui::DragFloat(name.c_str(), &data))
                            {
                                scriptInstance->SetFieldValue(name, data);
                            }
                        }
                    }
                }
            }
            else
            {
                if (scriptClassExists)
                {
                    ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
                    const auto& fields = entityClass->GetFields();

                    auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
                    for (const auto& [name, field] : fields)
                    {
                        // Field has not been set in editor
                        bool isSet = entityFields.find(name) != entityFields.end();
                        if (!isSet)
                        {
                            ScriptFieldInstance& fieldInstance = entityFields[name];
                            fieldInstance.Field = field;

                            // This works for geting default value like 'Speed = 45.4f' but maybe expensive memory alloc cuz of instance creation
//                            ScriptInstance instance = ScriptInstance(entityClass, entity);
//                            fieldInstance.template SetValue<float>(instance.GetFieldValue<float>(name));
//                            fox::info("data = %", fieldInstance.GetValue<float>());
                        }

                        ScriptFieldInstance& scriptField = entityFields.at(name);

                        // Display control to set it maybe
                        if (scriptField.Field.Type == ScriptFieldType::Float)
                        {
                            float data = scriptField.GetValue<float>();
                            if (ImGui::DragFloat(name.c_str(), &data))
                                AddLog("EROROROROROR !!!");
//                                scriptField.SetValue(data);
//                            Reflect::Any data = Reflect::Any(scriptField.GetValue<float>());
//                            if (ImGuiFieldDrawer::DrawField(field.Type, name, data))
//                                Reflect::Resolve<ScriptFieldInstance>()->GetDataMember("Value")->Set(scriptField, data);

                        }
                    }
                }
            }

            if (!scriptClassExists)
                ImGui::PopStyleColor();
        });

        DrawComponent<SpriteRenderer>("Sprite Renderer", entity);
        DrawComponent<CircleRenderer>("Circle Renderer", entity);
        DrawComponent<Rigidbody2D>("Rigidbody 2D", entity);
        DrawComponent<BoxCollider2D>("BoxCollider 2D", entity);
        DrawComponent<CircleCollider2D>("CircleCollider 2D", entity);


//       DrawComponent<AnimationPlayer>("AnimationPlayer", entity, [](AnimationPlayer& oAnimationPlayer)
//       {
////            static int count = 0;
////            int idx = 0;
////            auto& stringToId = animator.GetAnimationsIds();
////            auto& animations = animator.GetGraph();
////            for (auto& id : stringToId)
////            {
////                auto& animation = animations.node(id);
////                char buffer[256];
////                memset(buffer, 0, sizeof(buffer));
////                strcpy(buffer, animation.m_pAnimation->Name.get().c_str());
////
////                char label[256];
////                std::snprintf(label, 256, "Name##%d", idx);
////                if (ImGui::InputText(label, buffer, sizeof(buffer)))
////                    animation.m_pAnimation->Name = buffer;
////                idx++;
////            }
////
////            if (ImGui::Button("Add"))
////            {
////                animator.add_anim("Anim" + std::to_string(count++));
////            }
//       });
    }

    template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectedEntity.template has<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectedEntity.add<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}