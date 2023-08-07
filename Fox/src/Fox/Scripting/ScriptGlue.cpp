//
// Created by samuel on 03/10/22.
//

#include "ScriptGlue.hpp"
#include "ScriptEngine.hpp"

#include "Core/UUID.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Input/Input.hpp"
#include "Core/Logger/Logger.hpp"

#include "Scene/Scene.hpp"
#include "Ecs/Entity.hpp"
#include "Components.hpp"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "box2d/b2_body.h"
#include "Utils.hpp"
#include "Asset/AssetManager.hpp"
#include "Scene/Prefab.hpp"
#include "Utils/PlatformUtils.hpp"

namespace fox
{
    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
    static std::unordered_map<UUID, std::unordered_map<UUID, MonoDelegate*>> s_AnimatorEventDelegates;

#define FOX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Fox.InternalCalls::" #Name, (void*) Name)

    static MonoObject* GetScriptInstance(UUID entityID)
    {
        return ScriptEngine::GetManagedInstance(entityID);
    }

#pragma region Log

    void Log_LogMessage(int level, MonoString* formattedMessage)
    {
//        FOX_PROFILE_SCOPE();

        switch (level)
        {
            case typelog::INFO:		    FOX_INFO(Utils::MonoToString(formattedMessage));    break;
            case typelog::WARN:		    FOX_WARN(Utils::MonoToString(formattedMessage));    break;
            case typelog::ERROR:		FOX_ERROR(Utils::MonoToString(formattedMessage));   break;
            default:					FOX_INFO(Utils::MonoToString(formattedMessage));   break;
        }
    }
#pragma endregion

#pragma region AssetHandle

    bool AssetHandle_IsValid(AssetHandle* assetHandle)
    {
        return AssetManager::IsAssetHandleValid(*assetHandle);
    }

#pragma endregion

#pragma region Entity

    Entity GetEntity(uint64_t entityID)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene, "Active scene is null");
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity, "Entity is null");
        return entity;
    }

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        FOX_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        try {
            return s_EntityHasComponentFuncs.at(managedType)(entity);
        } catch (std::out_of_range& e) {
            return false;
        }
    }

    static uint64_t Entity_FindEntityByName(MonoString* name)
    {
//        FOX_PROFILE_SCOPE();

        char* nameCStr = mono_string_to_utf8(name);

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->FindEntityByName(nameCStr);
        mono_free(nameCStr);

        if (!entity)
            return 0;

        return entity.GetUUID();
    }

    static uint64_t Entity_Instantiate(uint64_t entityToInstantiate)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityToInstantiate);
        FOX_ASSERT(entity);

        return scene->CloneEntity(entity, {}).GetUUID();
    }

    uint64_t Entity_InstantiatePrefab(AssetHandle* prefabHandle)
    {
        Ref<Scene> scene = ScriptEngine::GetSceneContext();

        if (!AssetManager::IsAssetHandleValid(*prefabHandle))
            return 0;

        Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
        return scene->Instantiate(prefab).GetUUID();
    }

    uint64_t Entity_InstantiatePrefabWithTransform(AssetHandle* prefabHandle, glm::vec3* inPosition, glm::vec3* inRotation, glm::vec3* inScale)
    {
        Ref<Scene> scene = ScriptEngine::GetSceneContext();

        if (!AssetManager::IsAssetHandleValid(*prefabHandle))
            return 0;

        Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
        return scene->Instantiate(prefab, inPosition, inRotation, inScale).GetUUID();
    }

    uint64_t Entity_InstantiatePrefabWithPosition(AssetHandle* prefabHandle, glm::vec3* inPosition)
    {
        return Entity_InstantiatePrefabWithTransform(prefabHandle, inPosition, nullptr, nullptr);
    }

    static void Entity_Destroy(uint64_t entityToDestroy)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityToDestroy);
        FOX_ASSERT(entity);

        scene->SubmitToDestroyEntity(entity);
    }
#pragma endregion

#pragma region Name Component
    MonoString* NameComponent_GetName(uint64_t entityID)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        const auto& name = entity.get<NameComponent>();
        return mono_string_new(mono_domain_get(), name.name.c_str());
    }

    void NameComponent_SetName(uint64_t entityID, MonoString* tag)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        const auto& name = entity.get<NameComponent>();

        entity.get<NameComponent>().name = mono_string_to_utf8(tag);
    }
#pragma endregion

#pragma region Transform

    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        *outTranslation = entity.get<TransformComponent>().position;
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        entity.get<TransformComponent>().position = *translation;
    }

    void TransformComponent_GetRotation(uint64_t entityID, glm::vec3* outRotation)
    {
//        FOX_PROFILE_SCOPE();

        *outRotation = GetEntity(entityID).get<TransformComponent>().GetRotation();
    }

    void TransformComponent_SetRotation(uint64_t entityID, const glm::vec3* inRotation)
    {
//        FOX_PROFILE_SCOPE();

        GetEntity(entityID).get<TransformComponent>().SetRotation(*inRotation);
    }

    void TransformComponent_GetScale(uint64_t entityID, glm::vec3* outScale)
    {
//        FOX_PROFILE_SCOPE();

        *outScale = GetEntity(entityID).get<TransformComponent>().scale;
    }

    void TransformComponent_SetScale(uint64_t entityID, const glm::vec3* inScale)
    {
//        FOX_PROFILE_SCOPE();

        GetEntity(entityID).get<TransformComponent>().scale = *inScale;
    }
#pragma endregion

#pragma region Sprite Renderer

    void SpriteRendererComponent_GetColor(uint64_t entityID, glm::vec4* outTint)
    {
//        FOX_PROFILE_SCOPE();

        *outTint = GetEntity(entityID).get<SpriteRenderer>().Color;
    }

    void SpriteRendererComponent_SetColor(uint64_t entityID, const glm::vec4* tint)
    {
//        FOX_PROFILE_SCOPE();

        GetEntity(entityID).get<SpriteRenderer>().Color = *tint;
    }

    void SpriteRendererComponent_GetTilingFactor(uint64_t entityID, float* outTiling)
    {
//        FOX_PROFILE_SCOPE();

        *outTiling = GetEntity(entityID).get<SpriteRenderer>().TilingFactor;
    }

    void SpriteRendererComponent_SetTilingFactor(uint64_t entityID, const float* tiling)
    {
//        FOX_PROFILE_SCOPE();

        GetEntity(entityID).get<SpriteRenderer>().TilingFactor = *tiling;
    }
#pragma endregion

#pragma region Rigidbody2D
    void Rigidbody2DComponent_GetGravityScale(uint64_t entityID, float* outGravityScale)
	{
		// FOX_PROFILE_SCOPE();

		*outGravityScale = GetEntity(entityID).get<Rigidbody2D>().GravityScale;
	}

	void Rigidbody2DComponent_SetGravityScale(uint64_t entityID, const bool* gravityScale)
	{
		// FOX_PROFILE_SCOPE();

		auto& component = GetEntity(entityID).get<Rigidbody2D>();
		component.GravityScale = *gravityScale;
		if (component.RuntimeBody)
		{
			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetGravityScale(component.GravityScale);
		}
	}

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        auto& rb2d = entity.get<Rigidbody2D>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
//        FOX_PROFILE_SCOPE();

        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        auto& rb2d = entity.get<Rigidbody2D>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }
#pragma endregion

#pragma region Input

    bool Input_IsKeyPressed(KeyCode key)
    {
//        FOX_PROFILE_SCOPE();

        return Input::IsKeyPressed(key);
    }

    bool Input_IsKeyDown(KeyCode key)
    {
//        FOX_PROFILE_SCOPE();

        return Input::IsKeyDown(key);
    }

    bool Input_IsKeyReleased(KeyCode key)
    {
//        FOX_PROFILE_SCOPE();

        return Input::IsKeyReleased(key);
    }

    bool Input_IsMouseButtonPressed(Mouse button)
    {
//        FOX_PROFILE_SCOPE();

        return Input::IsMouseButtonPressed(button);
    }

    void Input_GetMousePosition(glm::vec2* outMousePosition)
    {
//        FOX_PROFILE_SCOPE();

        *outMousePosition = Input::GetMousePosition();
    }
#pragma endregion

#pragma region Animator

    void Animator_SubscribeToEvent(uint64_t entityID, uint64_t eventID, MonoDelegate* delegate)
    {
        Entity ent = GetEntity(entityID);
        s_AnimatorEventDelegates[ent.GetUUID()][eventID] = delegate;

        ent.get<Animator>().SubscribeToEvent(eventID, [entityID = ent.GetUUID()](UUID eventID) {

            MonoDelegate* delegate = s_AnimatorEventDelegates[entityID][eventID];
            MonoMethod* method = mono_get_delegate_invoke(mono_object_get_class((MonoObject*) delegate));

            uint64_t id = eventID;
            void* params = &id;

            MonoObject *ex;
            mono_runtime_invoke(method, delegate, nullptr, &ex);
            Utils::HandleException(ex);
        });
    }

    void Animator_UpdateDelegate(uint64_t entityID, uint64_t eventID, MonoDelegate* delegate)
    {
        Entity ent = GetEntity(entityID);
        s_AnimatorEventDelegates[ent.GetUUID()][eventID] = delegate;
    }

    void Animator_GetDelegate(uint64_t entityID, uint64_t eventID, MonoDelegate** delegate)
    {
        auto entityIt = s_AnimatorEventDelegates.find(entityID);
        if (entityIt == s_AnimatorEventDelegates.end())
        {
            delegate = nullptr;
            return;
        }

        auto eventIt = s_AnimatorEventDelegates[entityID].find(eventID);
        if (eventIt == s_AnimatorEventDelegates[entityID].end())
        {
            delegate = nullptr;
            return;
        }
        *delegate = s_AnimatorEventDelegates[entityID][eventID];
    }

    uint64_t Animator_GetHashID(MonoString* eventName)
    {
        return Hash::FNVHash(Utils::MonoToString(eventName));
    }

#pragma endregion

#pragma region Time

    float Time_deltaTime()
    {
        return Time::delta_time;
    }

#pragma endregion

    #include <cxxabi.h>  // needed for abi::__cxa_demangle

    std::shared_ptr<char> cppDemangle(const char *abiName)
    {
//        FOX_PROFILE_SCOPE();

        int status;
        char *ret = abi::__cxa_demangle(abiName, 0, 0, &status);

        /* NOTE: must free() the returned char when done with it! */
        std::shared_ptr<char> retval;
        retval.reset( (char *)ret, [](char *mem) { if (mem) free((void*)mem); } );
        return retval;
    }

    template<typename... Component>
    static void RegisterComponent()
    {
//        FOX_PROFILE_SCOPE();

        ([]()
        {
            std::string_view typeName = typeid(Component).name();
            auto demandleTypename = cppDemangle(typeName.data());
            std::string_view demandleTypenameView = demandleTypename.get();

            size_t pos = demandleTypenameView.find_last_of(':');
            std::string_view structName = demandleTypenameView.substr(pos + 1);
            std::string managedTypename = fox::format("Fox.%", structName);

            MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssembly()->AssemblyImage);
            if (!managedType)
            {
                FOX_CORE_ERROR("Could not find component type %", managedTypename);
                return;
            }

            s_EntityHasComponentFuncs[managedType] = [](Entity entity) { if (entity) return entity.has<Component>(); return false; };
        }(), ...);
    }

    template<typename... Component>
    static void RegisterComponent(ComponentGroup<Component...>)
    {
//        FOX_PROFILE_SCOPE();

        RegisterComponent<Component...>();
    }

    void ScriptGlue::RegisterComponents()
    {
//        FOX_PROFILE_SCOPE();

        RegisterComponent<NameComponent>();
        RegisterComponent(AllComponents{});
    }

    void ScriptGlue::ClearTypes()
    {
//        FOX_PROFILE_SCOPE();

        s_EntityHasComponentFuncs.clear();
    }

    void ScriptGlue::RegisterFunctions()
    {
//        FOX_PROFILE_SCOPE();

        FOX_ADD_INTERNAL_CALL(GetScriptInstance);
        FOX_ADD_INTERNAL_CALL(Log_LogMessage);
        FOX_ADD_INTERNAL_CALL(AssetHandle_IsValid);

        FOX_ADD_INTERNAL_CALL(Entity_HasComponent);
        FOX_ADD_INTERNAL_CALL(Entity_FindEntityByName);
        FOX_ADD_INTERNAL_CALL(Entity_Instantiate);
        FOX_ADD_INTERNAL_CALL(Entity_InstantiatePrefab);
        FOX_ADD_INTERNAL_CALL(Entity_InstantiatePrefabWithPosition);
        FOX_ADD_INTERNAL_CALL(Entity_Destroy);

        FOX_ADD_INTERNAL_CALL(NameComponent_GetName);
        FOX_ADD_INTERNAL_CALL(NameComponent_SetName);

        FOX_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        FOX_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
        FOX_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
        FOX_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
        FOX_ADD_INTERNAL_CALL(TransformComponent_GetScale);
        FOX_ADD_INTERNAL_CALL(TransformComponent_SetScale);

        FOX_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
        FOX_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
        FOX_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTilingFactor);
        FOX_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTilingFactor);

        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetGravityScale);
        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetGravityScale);

        FOX_ADD_INTERNAL_CALL(Input_IsKeyPressed);
        FOX_ADD_INTERNAL_CALL(Input_IsKeyDown);
        FOX_ADD_INTERNAL_CALL(Input_IsKeyReleased);
        FOX_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
        FOX_ADD_INTERNAL_CALL(Input_GetMousePosition);

        FOX_ADD_INTERNAL_CALL(Animator_SubscribeToEvent);
        FOX_ADD_INTERNAL_CALL(Animator_GetHashID);
        FOX_ADD_INTERNAL_CALL(Animator_UpdateDelegate);
        FOX_ADD_INTERNAL_CALL(Animator_GetDelegate);

        FOX_ADD_INTERNAL_CALL(Time_deltaTime);
    }

}
