//
// Created by samuel on 09/10/22.
//


#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "imgui_internal.h"
#include "Save for later/FPaths.hpp"
#include "Events/EventSystem.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "PropertyPanel.hpp"
#include "ImGui/ImGuiExtension.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Reflection/Core/Reflect.hpp"
#include "ImGui/ImGuiFieldDrawer.hpp"
#include "Core/EditorLogger.hpp"
#include "EditorEvent.hpp"

#include <filesystem>

namespace fox
{
    extern const std::filesystem::path g_AssetPath;

    PropertyPanel::PropertyPanel()
    {
        event::EventSystem::Get().On<OnSelectedEntityChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnSelectedEntityChange));
        event::EventSystem::Get().On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnContextChangeChange));
    }

    PropertyPanel::PropertyPanel(const ref<Scene>& context) : PropertyPanel()
    {
        SetContext(context);
    }

    PropertyPanel::~PropertyPanel()
    {
        m_pContext.reset();
    }

    void PropertyPanel::SetContext(const ref<Scene>& context)
    {
        m_pContext = context;
        m_SelectedEntity = {};
    }

    void PropertyPanel::OnImGui()
    {
        DrawLog("title");

        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity) {
                DrawComponents(m_SelectedEntity);
            }
        }
        ImGui::End();
    }

    void PropertyPanel::OnContextChangeChange(const OnContextChangeEvent& event)
    {
        m_pContext = event.Context;
        m_SelectedEntity = {};
    }

    void PropertyPanel::OnSelectedEntityChange(const OnSelectedEntityChangeEvent& event)
    {
        m_SelectedEntity = event.m_oEntity;
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

            ImGuiFieldDrawer::BeginProperties();
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
            ImGuiFieldDrawer::EndProperties();
        });
    }

    template<typename T>
    static void DrawFieldIf(ScriptFieldType type, ScriptFieldInstance& fieldInstance, std::string name)
    {
        if (fieldInstance.Field.Type != type)
            return;

        Reflect::Any data = fieldInstance.GetValue<T>();
        if (ImGuiFieldDrawer::DrawField(fieldInstance.Field.Type, name, data))
        {
//            if (type == ScriptFieldType::Entity)
//            {
//                Entity* entity = data.template TryCast<Entity>();
////                fox::trace("En: %", entity->GetName());
//                fieldInstance.SetValue<uint64_t>((uint64_t) entity->GetUUID());
//            }
//            else
//            {
                fieldInstance.SetValue(data.Get(), data.GetType()->GetSize());
//            }
        }
    }

    template<typename T>
    static void DrawFieldIf(ScriptFieldType type, ScriptField field, ref<ScriptInstance>& instance, std::string name)
    {
        if (field.Type != type)
            return;

        Reflect::Any data = instance->template GetFieldValue<T>(name);
        if (ImGuiFieldDrawer::DrawField(field.Type, name, data))
        {
            instance->SetFieldValue<T>(name, *data.template TryCast<T>());
        }
    }

    void PropertyPanel::DrawComponents(Entity entity)
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
            bool IsSriptClassExists;

            static char buffer[64];
            strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));

            if (!scriptClassExists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

            if (ImGui::InputText("Class", buffer, sizeof(buffer)))
            {
                component.ClassName = buffer;
                IsSriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);
                if (!IsSriptClassExists)
                    return;
            }

            if (!scriptClassExists)
            {
                ImGui::PopStyleColor();
                return;
            }

            // Fields
            bool sceneRunning = scene->IsRunning();
            if (sceneRunning)
            {
                ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
                if (scriptInstance)
                {
                    const auto& fields = scriptInstance->GetScriptClass()->GetFields();

                    ImGuiFieldDrawer::BeginProperties();
                    for (const auto& [name, field] : fields)
                    {
                        DrawFieldIf<float>(ScriptFieldType::Float, field, scriptInstance, name);
                        DrawFieldIf<int>(ScriptFieldType::Int, field, scriptInstance, name);
                        DrawFieldIf<double>(ScriptFieldType::Double, field, scriptInstance, name);
                        DrawFieldIf<bool>(ScriptFieldType::Bool, field, scriptInstance, name);
                        DrawFieldIf<glm::vec2>(ScriptFieldType::Vector2, field, scriptInstance, name);
                        DrawFieldIf<glm::vec3>(ScriptFieldType::Vector3, field, scriptInstance, name);
                        DrawFieldIf<glm::vec4>(ScriptFieldType::Vector4, field, scriptInstance, name);

//                        if (field.Type == ScriptFieldType::Float)
//                        {
//                            float data = scriptInstance->GetFieldValue<float>(name);
//                            if (ImGui::DragFloat(name.c_str(), &data))
//                            {
//                                scriptInstance->SetFieldValue(name, data);
//                            }
//                        }
                    }
                    ImGuiFieldDrawer::EndProperties();
                }
            }
            else
            {
                if (scriptClassExists)
                {
                    ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
                    const auto& fields = entityClass->GetFields();

                    if (fields.size() != 0)
                    {
                        auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

                        ImGuiFieldDrawer::BeginProperties();
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
                            DrawFieldIf<float>(ScriptFieldType::Float, scriptField, name);
                            DrawFieldIf<int>(ScriptFieldType::Int, scriptField, name);
                            DrawFieldIf<double>(ScriptFieldType::Double, scriptField, name);
                            DrawFieldIf<bool>(ScriptFieldType::Bool, scriptField, name);
                            DrawFieldIf<glm::vec2>(ScriptFieldType::Vector2, scriptField, name);
                            DrawFieldIf<glm::vec3>(ScriptFieldType::Vector3, scriptField, name);
                            DrawFieldIf<glm::vec4>(ScriptFieldType::Vector4, scriptField, name);
                            DrawFieldIf<Entity>(ScriptFieldType::Entity, scriptField, name);
                        }
                        ImGuiFieldDrawer::EndProperties();
                    }
                }
            }
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
    void PropertyPanel::DisplayAddComponentEntry(const std::string& entryName) {
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
