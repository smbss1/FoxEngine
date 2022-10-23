//
// Created by samuel on 09/10/22.
//

#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "imgui_internal.h"
#include "Events/EventSystem.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "PropertyPanel.hpp"
#include "ImGui/ImGuiExtension.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Reflection/Core/Reflect.hpp"
#include "ImGui/ImGuiFieldDrawer.hpp"
#include "EditorEvent.hpp"
#include "Scripting/FieldStorage.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Asset/AssetManager.hpp"
#include "Scene/Prefab.hpp"

#include <filesystem>

namespace fox
{
    PropertyPanel::PropertyPanel()
    {
        event::EventSystem::Get().On<OnSelectedEntityChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnSelectedEntityChange));
        event::EventSystem::Get().On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnContextChangeChange));
    }

    PropertyPanel::PropertyPanel(const Ref<Scene>& context) : PropertyPanel()
    {
        SetContext(context);
    }

    PropertyPanel::~PropertyPanel()
    {
        m_pContext.Reset();
    }

    void PropertyPanel::SetContext(const Ref<Scene>& context)
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

//                        FOX_CORE_INFO("Name: %", member->GetName());
//                        FOX_CORE_INFOINFOINFO("Type: %", type->GetName());
//                        FOX_CORE_INFO("Value: %", value.Get());

                if (ImGuiFieldDrawer::Draw(type, member->GetName(), value))
                {
                    member->Set(component, value);
                }
            }
            ImGuiFieldDrawer::EndProperties();
        });
    }

    void GetFieldValue(const FieldStorage& storage, Reflect::Any& data)
    {
        switch (storage.GetField()->Type.NativeType)
        {
            case ScriptFieldType::Float:
                data = storage.GetValue<float>();
                break;
            case ScriptFieldType::Int:
                data = storage.GetValue<int>();
                break;
            case ScriptFieldType::Double:
                data = storage.GetValue<double>();
                break;
            case ScriptFieldType::Bool:
                data = storage.GetValue<bool>();
                break;
            case ScriptFieldType::Vector2:
                data = storage.GetValue<glm::vec2>();
                break;
            case ScriptFieldType::Vector3:
                data = storage.GetValue<glm::vec3>();
                break;
            case ScriptFieldType::Vector4:
                data = storage.GetValue<glm::vec4>();
                break;
            case ScriptFieldType::Entity:
                data = storage.GetValue<UUID>();
                break;
            case ScriptFieldType::Prefab:
                data = storage.GetValue<AssetHandle>();
                break;
            case ScriptFieldType::AssetHandle:
                data = storage.GetValue<AssetHandle>();
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
            bool IsScriptClassExists;

            static char buffer[64];
            strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));

            if (!scriptClassExists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

            if (ImGui::InputText("Class", buffer, sizeof(buffer)))
            {
                component.ClassName = buffer;
                IsScriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);
                if (!IsScriptClassExists)
                    return;
            }

            if (!scriptClassExists)
            {
                ImGui::PopStyleColor();
                return;
            }

            // TODO: Maybe the field storage is not unique to one instance, it's may shared ? (need verification)
            // Fields
            if (scriptClassExists)
            {
                ManagedClass* entityClass = ScriptEngine::GetEntityClass(component.ClassName);
                const auto& fields = entityClass->Fields;
                if (fields.size() != 0)
                {
                    ImGuiFieldDrawer::BeginProperties();
                    for (const auto fieldID : fields)
                    {
                        ManagedField* field = ScriptCache::GetFieldByID(fieldID);
                        if (!field->IsWritable())
                            continue;

                        FieldStorage& storage = *ScriptCache::GetFieldStorage(fieldID);
                        std::string name = field->DisplayName;

                        Reflect::Any data;
                        GetFieldValue(storage, data);

                        switch (storage.GetField()->Type.NativeType)
                        {
                            case ScriptFieldType::Float:
                            case ScriptFieldType::Int:
                            case ScriptFieldType::Double:
                            case ScriptFieldType::Bool:
                            case ScriptFieldType::Vector2:
                            case ScriptFieldType::Vector3:
                            case ScriptFieldType::Vector4:
                            {
                                if (ImGuiFieldDrawer::DrawField(storage.GetField()->Type.NativeType, name, data))
                                    storage.SetValue(data.Get(), data.GetType()->GetSize());
                                break;
                            }

                            case ScriptFieldType::Entity:
                            {
                                ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
                                Entity entityRef = scene->GetEntityByUUID(*data.template TryCast<UUID>());
                                ImGui::Button(entityRef ? entityRef.GetName().c_str() : "No Reference", ImVec2(100.0f, 0.0f));
                                ImGuiFieldDrawer::EndPropertyGrid();

                                if (ImGui::BeginDragDropTarget())
                                {
                                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy")) {
                                        size_t count = payload->DataSize / sizeof(UUID);
                                        if (count == 1)
                                        {
                                            UUID droppedEntityID = *(((UUID*)payload->Data));
                                            Entity droppedEntity = scene->GetEntityByUUID(droppedEntityID);
                                            storage.SetValue(droppedEntity.GetUUID());
                                        }
                                        else
                                            FOX_INFO_TAG("PropertyPanel", "Cannot set reference of multiple entityin a field which need only one reference");
                                    }
                                    ImGui::EndDragDropTarget();
                                }
                                break;
                            }
                            case ScriptFieldType::Prefab:
                            {
                                ImGuiFieldDrawer::BeginPropertyGrid(name.c_str(), nullptr);
                                AssetHandle assetHandle = *data.template TryCast<AssetHandle>();
                                Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(assetHandle);
                                ImGui::Button(prefab ? AssetManager::GetFileSystemPath(AssetManager::GetMetadata(assetHandle)).stem().c_str() : "No Reference", ImVec2(100.0f, 0.0f));
                                ImGuiFieldDrawer::EndPropertyGrid();

                                if (ImGui::BeginDragDropTarget())
                                {
                                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                                    {
                                        const char *path = (const char *) payload->Data;
                                        std::filesystem::path prefabPath = Project::AssetsDir() / path;

                                        if (prefabPath.extension() == ".foxprefab")
                                        {
                                            storage.SetValue(AssetManager::GetAssetHandleFromFilePath(prefabPath));
                                        }
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
