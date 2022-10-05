//
// Created by samuel on 25/06/2021.
//

#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "imgui_internal.h"
#include "Utils/Path.hpp"
#include "FPaths.hpp"
#include "Events/EventSystem.hpp"
#include "../EditorEvent.hpp"
#include <filesystem>
#include "glm/gtc/type_ptr.hpp"

#include "Components.hpp"
#include "SceneHierarchyPanel.hpp"
#include "ImGuiExtension.hpp"
#include "Scripting/ScriptEngine.hpp"

namespace fox
{
    extern const std::filesystem::path g_AssetPath;

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

        event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));

        // if (!m_pScriptLib->GetHandle()) {

        //     std::unordered_map<size_t, std::string> (*GetScriptsNames)();
        //     std::unordered_map<size_t, ScriptCreator> (*GetScripts)();
        //     fox::info("Open the native script library");

        //     try {
        //         std::string strLibPath = FPaths::ProjectDir() + "/libfox_native_script" DL_EXT;
        //         m_pScriptLib->open(strLibPath);
        //         m_pScriptLib->sym("GetScriptsNames", GetScriptsNames);
        //         m_pScriptLib->sym("GetScripts", GetScripts);
        //         m_vScriptsNames = GetScriptsNames();
        //         m_vScripts = GetScripts();
        //         context->GetApp().SetScriptsArray(m_vScripts);
        //         for (auto name : m_vScriptsNames) {
        //             fox::info("Script: '%'", name.second);
        //         }
        //     }
        //     catch (std::exception &e) {
        //         fox::error("%", e.what());
        //     }
        // }
    }

    void SceneHierarchyPanel::OnImGui()
    {
        ImGui::Begin("Scene Hierarchy");

        if (m_pContext)
        {
            m_pContext->m_Registry.each([&](auto entityID)
           {
               Entity entity { entityID , m_pContext.get() };
               DrawEntityNode(entity);
           });

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
                m_SelectedEntity = {};
                event::EventSystem::Get().Emit(OnSelectedEntityChangeEvent(m_SelectedEntity));
            }

            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, 1, false))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_pContext->NewEntity();

                ImGui::EndPopup();
            }
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

        if (entity.has<T>())
		{
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
	}

    void SceneHierarchyPanel::DrawComponents(Entity& entity)
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

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			imgui::DrawVec3Control("Position", component.position);
			glm::vec3 rotation = glm::degrees(component.rotation);
			imgui::DrawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			imgui::DrawVec3Control("Scale", component.scale, 1.0f);
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

        DrawComponent<ScriptComponent>("Script", entity, [](auto& component)
        {
            bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

            static char buffer[64];
            strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));

            if (!scriptClassExists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

            if (ImGui::InputText("Class", buffer, sizeof(buffer)))
                component.ClassName = buffer;

//            // Fields
//            bool sceneRunning = scene->IsRunning();
//            if (sceneRunning)
//            {
//                Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
//                if (scriptInstance)
//                {
//                    const auto& fields = scriptInstance->GetScriptClass()->GetFields();
//                    for (const auto& [name, field] : fields)
//                    {
//                        if (field.Type == ScriptFieldType::Float)
//                        {
//                            float data = scriptInstance->GetFieldValue<float>(name);
//                            if (ImGui::DragFloat(name.c_str(), &data))
//                            {
//                                scriptInstance->SetFieldValue(name, data);
//                            }
//                        }
//                    }
//                }
//            }
//            else
//            {
//                if (scriptClassExists)
//                {
//                    Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
//                    const auto& fields = entityClass->GetFields();
//
//                    auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
//                    for (const auto& [name, field] : fields)
//                    {
//                        // Field has been set in editor
//                        if (entityFields.find(name) != entityFields.end())
//                        {
//                            ScriptFieldInstance& scriptField = entityFields.at(name);
//
//                            // Display control to set it maybe
//                            if (field.Type == ScriptFieldType::Float)
//                            {
//                                float data = scriptField.GetValue<float>();
//                                if (ImGui::DragFloat(name.c_str(), &data))
//                                    scriptField.SetValue(data);
//                            }
//                        }
//                        else
//                        {
//                            // Display control to set it maybe
//                            if (field.Type == ScriptFieldType::Float)
//                            {
//                                float data = 0.0f;
//                                if (ImGui::DragFloat(name.c_str(), &data))
//                                {
//                                    ScriptFieldInstance& fieldInstance = entityFields[name];
//                                    fieldInstance.Field = field;
//                                    fieldInstance.SetValue(data);
//                                }
//                            }
//                        }
//                    }
//                }
//            }

            if (!scriptClassExists)
                ImGui::PopStyleColor();
        });

        DrawComponent<SpriteRenderer>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const char* path = (const char*)payload->Data;
                    fox::info("rdhtrhrth");
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					ref<Texture2D> texture = Texture2D::Create(texturePath.string());
                    component.Sprite = texture;

                    // if (texture->IsLoaded())
					// 	component.Sprite = texture;
					// else
					// 	fox::warn("Could not load texture %", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

        DrawComponent<CircleRenderer>("Circle Renderer", entity, [](auto& component)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
            ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
            ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
        });

        DrawComponent<Rigidbody2D>("Rigidbody 2D", entity, [](auto& component)
        {
            const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
            const char* currentBodyTypeString = bodyTypeString[(int)component.Type];
            if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
            {
                for (int i = 0; i < 3; ++i)
                {
                    bool isSelected = currentBodyTypeString == bodyTypeString[i];
                    if (ImGui::Selectable(bodyTypeString[i], isSelected))
                    {
                        currentBodyTypeString = bodyTypeString[i];
                        component.Type = (Rigidbody2D::BodyType)i;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
        });

        DrawComponent<BoxCollider2D>("BoxCollider 2D", entity, [](auto& component)
        {
            ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
            ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, 1.0f);
        });

        DrawComponent<CircleCollider2D>("CircleCollider 2D", entity, [](auto& component)
        {
            ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
            ImGui::DragFloat("Radius", &component.Radius);
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, 1.0f);
        });


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
