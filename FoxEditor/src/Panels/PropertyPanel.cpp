//
// Created by samuel on 09/10/22.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "Core/Application.hpp"
#include "Components.hpp"
#include "imgui_internal.h"
#include "Events/EventSystem.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "PropertyPanel.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Reflection/Core/Reflect.hpp"
#include "ImGui/ImGuiFieldDrawer.hpp"
#include "EditorEvent.hpp"
#include "Scripting/FieldStorage.hpp"
#include "Scene/EntitySerializer.hpp"
#include "Asset/AssetManager.hpp"
#include "Scene/Prefab.hpp"
#include "ImGui/ImGuiExtensions.hpp"
#include "ImGui/IconsFontAwesome5Brands.hpp"
#include "ImGui/IconsFontAwesome5.hpp"
#include "Physics2D.hpp"

#include "Core/Base.hpp"

namespace fox
{
    PropertyPanel::PropertyPanel()
    {
        event::EventSystem::On<OnSelectedEntityChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnSelectedEntityChange));
        event::EventSystem::On<OnContextChangeEvent>(FOX_BIND_EVENT_FN(PropertyPanel::OnContextChangeChange));
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

    void PropertyPanel::OnImGui(bool& isOpen)
    {
        ImGui::Begin("Properties", &isOpen);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));
        {
            if (m_SelectedEntity) {
                DrawComponents(m_SelectedEntity);
            }
        }
        ImGui::PopStyleVar();
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
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool manual = false)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        if (!entity.has<T>())
            return;

        float lineHeight = 0;
        bool open = false;
        auto& component = entity.get<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4, 4});
        {
            defer({ ImGui::PopStyleVar(); });

            lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            open = ImGui::TreeNodeEx((void *) typeid(T).hash_code(), treeNodeFlags, name.c_str());
        }
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
            if (!manual)
                UI::BeginProperties();
            uiFunction(component);
            if (!manual)
                UI::EndProperties();
            ImGui::TreePop();
        }

        if (removeComponent)
            entity.remove<T>();
    }

    //template<typename T>
    //static void DrawComponent(const std::string& name, Entity entity, std::function<bool(const Reflect::TypeDescriptor*, const std::string&, Reflect::Any&)> drawField)
    //{
    //    DrawComponent<T>(name, entity, [](auto& component) {
    //        auto reflect = Reflect::Resolve<T>();
    //        auto members = reflect->GetDataMembers();

    //        for (Reflect::DataMember* member : members)
    //        {
    //            auto value = member->Get(component);
    //            const Reflect::TypeDescriptor* type = member->GetType();
    //            if (drawField(type, member->GetName(), value))
    //            {
    //                member->Set(component, value);
    //            }
    //        }
    //        });
    //}

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
                if (ImGuiFieldDrawer::Draw(type, member->GetName(), value))
                {
                    member->Set(component, value);
                }
            }
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
                if (!storage.HasValue())
                    data = UUID::Empty();
                else
                    data = storage.GetValue<UUID>();
                break;
            case ScriptFieldType::Prefab:
                data = storage.GetValue<AssetHandle>();
                break;
            case ScriptFieldType::AssetHandle:
                data = storage.GetValue<AssetHandle>();
                break;
            default:
                break;
        }
    }

    void PropertyPanel::DrawComponents(Entity entity)
    {
        if (entity.has<NameComponent>())
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, entity.get<NameComponent>().name.c_str());

            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                entity.get<NameComponent>().name = buffer;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            defer({ ImGui::EndPopup(); });

            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<ScriptComponent>("Script Component");
            DisplayAddComponentEntry<SpriteRenderer>("Sprite Renderer");
            DisplayAddComponentEntry<Rigidbody2D>("Rigidbody 2D");
            DisplayAddComponentEntry<BoxCollider2D>("BoxCollider 2D");
            DisplayAddComponentEntry<CircleCollider2D>("CircleCollider 2D");
            DisplayAddComponentEntry<CircleRenderer>("Circle Renderer");
            DisplayAddComponentEntry<ParticleSystem>("Particle System");
            DisplayAddComponentEntry<Animator>("Animator");
        }
        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity);
        DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComp)
        {
            auto& camera = cameraComp.camera;

            UI::Property("Primary", cameraComp.Primary);

            int result = -1;
            const char* projectionTypeString[] = { "Perspective", "Orthographic" };
            if (UI::Property("Projection", projectionTypeString, 2, (int)camera.GetProjectionType(), result))
                camera.SetProjectionType((SceneCamera::ProjectionType)result);

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float size = camera.GetOrthographicSize();
                if (UI::Property("Size", size))
                    camera.SetOrthographicSize(size);

                float near = camera.GetOrthographicNearClip();
                if (UI::Property("Near", near))
                    camera.SetOrthographicNearClip(near);

                float far = camera.GetOrthographicFarClip();
                if (UI::Property("Far", far))
                    camera.SetOrthographicFarClip(far);

                UI::Property("Fixed Aspect Ratio", cameraComp.FixedAspectRatio);
            }
            else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (UI::Property("Size", fov))
                    camera.SetPerspectiveVerticalFOV(glm::radians(fov));

                float near = camera.GetPerspectiveNearClip();
                if (UI::Property("Near", near))
                    camera.SetPerspectiveNearClip(near);

                float far = camera.GetPerspectiveFarClip();
                if (UI::Property("Far", far))
                    camera.SetPerspectiveFarClip(far);
            }
        });
        DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_pContext](auto& component) mutable
        {
            bool IsScriptClassExists;
            bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

            static char buffer[64];
            strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));

            if (!scriptClassExists)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

            std::string oldClassName = component.ClassName;
            if (UI::PropertyInput("Class", buffer, 64))
            {
                component.ClassName = buffer;
                IsScriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);
                if (!IsScriptClassExists)
                {
                    if (!scriptClassExists)
                        ImGui::PopStyleColor();

                    bool wasCleared = scriptClassExists;
                    if (wasCleared)
                        component.ClassName = oldClassName; // NOTE(Peter): We need the old asset handle to properly shutdown an entity (at least during runtime)

                    ScriptEngine::ShutdownScriptEntity(entity);

                    if (wasCleared)
                        component.ClassName = "";
                    return;
                }
                else
                {
                    ScriptEngine::InitializeScriptEntity(entity);
                }
            }

            if (!scriptClassExists)
            {
                ImGui::PopStyleColor();
                return;
            }

            // TODO: Maybe the field storage is not unique to one instance, it's may shared ? (need verification)
            // Fields
            ManagedClass *entityClass = ScriptEngine::GetEntityClass(component.ClassName);
            const auto &fields = entityClass->Fields;
            if (fields.empty())
                return;
            for (const auto fieldID : fields)
            {
                ManagedField *field = ScriptCache::GetFieldByID(fieldID);
                if (!field->IsWritable())
                    continue;

                FieldStorage& storage = *ScriptEngine::GetFieldStorage(entity, fieldID);
                std::string name = field->DisplayName;

                Reflect::Any data;
                GetFieldValue(storage, data);

                switch (storage.GetField()->Type.NativeType)
                {
                    default:
                        if (ImGuiFieldDrawer::DrawField(storage.GetField()->Type.NativeType, name, data))
                            storage.SetValue(data.Get(), data.GetType()->GetSize());
                        break;
                    case ScriptFieldType::Entity:
                    {
                        UI::BeginPropertyGrid(name.c_str(), nullptr);
                        Entity entityRef = scene->TryGetEntityByUUID(*data.template TryCast<UUID>());
                        ImGui::Button(entityRef ? entityRef.GetName().c_str() : "No Reference", ImVec2(100.0f, 0.0f));
                        UI::EndPropertyGrid();

                        if (ImGui::BeginDragDropTarget())
                        {
                            defer({ ImGui::EndDragDropTarget(); });
                            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SceneHierarchy"))
                            {
                                size_t count = payload->DataSize / sizeof(UUID);
                                if (count > 1) {
                                    FOX_INFO_TAG("PropertyPanel", "Cannot set reference of multiple entityin a field which need only one reference");
                                    continue;
                                }

                                UUID droppedEntityID = *(((UUID *) payload->Data));
                                Entity droppedEntity = scene->GetEntityByUUID(droppedEntityID);
                                storage.SetValue(droppedEntity.GetUUID());
                            }
                        }
                        break;
                    }
                    case ScriptFieldType::Prefab:
                    {
                        AssetHandle assetHandle = *data.template TryCast<AssetHandle>();
                        UI::AssetField(name, assetHandle);

                        UI::HandleContentBrowserPayloadCustom({".foxprefab"},
                                                              [&storage](fs::path &filepath)
                                                              {
                                                                  storage.SetValue(
                                                                      AssetManager::GetAssetHandleFromFilePath(
                                                                          filepath));
                                                              });
                        break;
                    }
                }
            }
        });
        DrawComponent<SpriteRenderer>("Sprite Renderer", entity, [](SpriteRenderer& comp)
        {
            AssetLink link;
            link.Handle = comp.Sprite;
            link.Type = (uint16_t)AssetType::Texture;
            if (UI::AssetField("Sprite", link, true))
            {
                comp.Sprite = link.Handle;
            }
            UI::PropertyColor("Color", comp.Color);
            UI::Property("Tiling Factor", comp.TilingFactor);
        });
        DrawComponent<CircleRenderer>("Circle Renderer", entity);
        DrawComponent<Rigidbody2D>("Rigidbody 2D", entity, [](Rigidbody2D& comp)
        {
            const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
            int result = -1;
            if (UI::Property("Body Type", bodyTypeString, 3, (int) comp.Type, result))
            {
                comp.Type = (Rigidbody2D::BodyType) result;
            }

            auto layers = Physics2D::GetLayersName();
            int resultLayer = -1;
            if (UI::Property("Layer", layers.data(), layers.size(), (int) comp.LayerID, resultLayer))
            {
                comp.LayerID = resultLayer;
            }

            if (comp.Type == Rigidbody2D::BodyType::Dynamic)
            {
                UI::Property("Fixed Rotation", comp.FixedRotation);
                UI::Property("Gravity Scale", comp.GravityScale);
                UI::Property("Mass", comp.Mass);
                UI::Property("Is Bullet", comp.IsBullet);
                UI::Property("Angular Drag", comp.AngularDrag);
                UI::Property("Linear Drag", comp.LinearDrag);
            }
        });
        DrawComponent<BoxCollider2D>("BoxCollider 2D", entity);
        DrawComponent<CircleCollider2D>("CircleCollider 2D", entity);

        DrawComponent<ParticleSystem>("Particle System", entity, [](ParticleSystem& component)
        {
            ParticleProps& props = component.ParticleSettings;
            UI::Property("Play On Start", component.PlayOnStart);
            UI::Property("Life Time", props.LifeTime, 1.0f, 0.01f, FLT_MAX);
            if (UI::Property("Max Particles", component.MaxParticles))
            {
                component.Resize();
            }
            UI::PropertyColor("Color Begin", props.ColorBegin);
            UI::PropertyColor("Color End", props.ColorEnd);
            UI::Property("Size Begin", props.SizeBegin, 1.0f, 0.0f, FLT_MAX);
            UI::Property("Size End", props.SizeEnd, 1.0f, 0.0f, FLT_MAX);
            UI::Property("Size Variation", props.SizeVariation);
            UI::Property("Rotation Speed", props.RotationSpeed);
            UI::Property("Speed", props.Speed);
            UI::Property("Velocity", props.Velocity);
            UI::Property("Velocity Variation", props.VelocityVariation);
            UI::Property("Rate Over Time", props.RateOverTime, 1.0f, 0.0f, FLT_MAX);
        });

        DrawComponent<Animator>("Animator", entity, [](Animator& component)
        {
            Ref<Animation> anim = component.CurrentAnimation;
            if (!anim)
                return;

            Ref<Frame>& frame = anim->CurrentFrame;
            if (frame && AssetManager::IsAssetHandleValid(frame->Texture))
            {
                UI::PreviewTexture(frame->Texture, {".png"});
                ImGui::SameLine();
            }
            else if (frame)
            {
                UI::DropBoxAsset(frame->Texture, {".png"});
            }
            ImGui::BeginChild("##bjkbjfuhhvv", ImVec2(0, 100));
            {
                if (ImGui::Button(component.Play ? ICON_FA_STOP : ICON_FA_PLAY))
                {
                    component.TogglePlay();
                }
                ImGui::SameLine();

                bool lastLoopState = anim->Loop;
                if (lastLoopState)
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(15, 15, 15, 255));
                if (ImGui::Button(ICON_FA_SPINNER))
                {
                    anim->Loop = !anim->Loop;
                }
                if (lastLoopState)
                    ImGui::PopStyleColor();

                ImGui::SameLine();
                int max = anim->Frames.empty() ? 0 : anim->Frames.size() - 1;
                int currentIndex = anim->GetCurrentIndex();
                if (UI::Slider("##CurrentIndex", currentIndex, 0, max))
                {
                    anim->SetCurrentIndex(currentIndex);
                }
                if (ImGui::Button("Add Frame"))
                {
                    Ref<Frame> frame = new_ref<Frame>();
                    anim->Frames.push_back(frame);
                    anim->SetCurrentIndex(anim->Frames.size() - 1);
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove Frame"))
                {
                    if (anim->CurrentFrame)
                    {
                        anim->Frames.erase(std::remove(anim->Frames.begin(), anim->Frames.end(), anim->CurrentFrame), anim->Frames.end());
                        anim->SetCurrentIndex(anim->GetCurrentIndex() - 1);
                    }
                }
            }
            ImGui::EndChild();
        }, true);

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
