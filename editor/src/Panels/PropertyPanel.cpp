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

    void GetFieldValue(ScriptFieldInstance scriptField, Reflect::Any& data)
    {
        switch (scriptField.Field.Type)
        {
            case ScriptFieldType::Float:
                data = scriptField.GetValue<float>();
                break;
            case ScriptFieldType::Int:
                data = scriptField.GetValue<int>();
                break;
            case ScriptFieldType::Double:
                data = scriptField.GetValue<double>();
                break;
            case ScriptFieldType::Bool:
                data = scriptField.GetValue<bool>();
                break;
            case ScriptFieldType::Vector2:
                data = scriptField.GetValue<glm::vec2>();
                break;
            case ScriptFieldType::Vector3:
                data = scriptField.GetValue<glm::vec3>();
                break;
            case ScriptFieldType::Vector4:
                data = scriptField.GetValue<glm::vec4>();
                break;
        }
    }

    void GetFieldValue(ScriptField field, ref<ScriptInstance> instance, Reflect::Any& data)
    {
        switch (field.Type)
        {
            case ScriptFieldType::Float:
                data = instance->GetFieldValue<float>(field.Name);
                break;
            case ScriptFieldType::Int:
                data = instance->GetFieldValue<int>(field.Name);
                break;
            case ScriptFieldType::Double:
                data = instance->GetFieldValue<double>(field.Name);
                break;
            case ScriptFieldType::Bool:
                data = instance->GetFieldValue<bool>(field.Name);
                break;
            case ScriptFieldType::Vector2:
                data = instance->GetFieldValue<glm::vec2>(field.Name);
                break;
            case ScriptFieldType::Vector3:
                data = instance->GetFieldValue<glm::vec3>(field.Name);
                break;
            case ScriptFieldType::Vector4:
                data = instance->GetFieldValue<glm::vec4>(field.Name);
                break;
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


            // TODO: Find a wayto 'combine' the drawing of the fields
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
                        Reflect::Any data;
                        GetFieldValue(field, scriptInstance, data);

                        switch (field.Type)
                        {
                            case ScriptFieldType::Float:
                            case ScriptFieldType::Int:
                            case ScriptFieldType::Double:
                            case ScriptFieldType::Bool:
                            case ScriptFieldType::Vector2:
                            case ScriptFieldType::Vector3:
                            case ScriptFieldType::Vector4:
                            {
                                if (ImGuiFieldDrawer::DrawField(field.Type, name, data))
                                    scriptInstance->SetFieldValue(name, data.Get());
                                break;
                            }

                            case ScriptFieldType::Entity:
                            {
                                ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
                                MonoObject* entityObj = scriptInstance->template GetFieldObject<MonoObject>(name);
                                Entity entityRef = ScriptEngine::GetEntityInstanceFromMonoObject(entityObj);
                                ImGui::Button(entityRef ? entityRef.GetName().c_str() : "No Reference", ImVec2(100.0f, 0.0f));

                                // commented because on runtime we don't want a c++ Entity class but a C# instance of Entity class
//                                if (ImGui::BeginDragDropTarget())
//                                {
//                                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy")) {
//                                        auto* e = static_cast<Entity *>(payload->Data);
//                                        scriptInstance->template SetFieldValue<UUID>(name, e->GetUUID());
//                                    }
//                                    ImGui::EndDragDropTarget();
//                                }
                                ImGuiFieldDrawer::EndPropertyGrid();
                                break;
                            }
                            default:
                                break;
                        }
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

                            Reflect::Any data;
                            ScriptFieldInstance& scriptField = entityFields.at(name);
                            GetFieldValue(scriptField, data);

                            switch (scriptField.Field.Type)
                            {
                                case ScriptFieldType::Float:
                                case ScriptFieldType::Int:
                                case ScriptFieldType::Double:
                                case ScriptFieldType::Bool:
                                case ScriptFieldType::Vector2:
                                case ScriptFieldType::Vector3:
                                case ScriptFieldType::Vector4:
                                {
                                    if (ImGuiFieldDrawer::DrawField(scriptField.Field.Type, name, data))
                                        scriptField.SetValue(data.Get(), data.GetType()->GetSize());
                                    break;
                                }

                                case ScriptFieldType::Entity:
                                {
                                    ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
                                    Entity entityRef = scene->GetEntityByUUID(scriptField.GetValue<UUID>());
                                    ImGui::Button(entityRef ? entityRef.GetName().c_str() : "No Reference", ImVec2(100.0f, 0.0f));
                                    ImGuiFieldDrawer::EndPropertyGrid();

                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy")) {
                                            auto* e = static_cast<Entity *>(payload->Data);
                                            scriptField.SetValue<UUID>(e->GetUUID());
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
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
