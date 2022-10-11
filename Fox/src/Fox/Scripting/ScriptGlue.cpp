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
#include "mono/metadata/object.h"
//#include "mono/metadata/tabledefs.h"

#include "box2d/b2_body.h"
#include "Utils.hpp"


namespace fox
{
    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define FOX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Fox.InternalCalls::" #Name, (void*) Name)

    static MonoObject* GetScriptInstance(UUID entityID)
    {
        return ScriptEngine::GetManagedInstance(entityID);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Logging ////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////

    void Log_LogMessage(typelog level, MonoString* formattedMessage)
    {
//        FOX_PROFILE_SCOPE();

        switch (level)
        {
            case typelog::TRACE:		fox::trace(Utils::MonoToString(formattedMessage));   break;
            case typelog::DEBUG:		fox::debug(Utils::MonoToString(formattedMessage));   break;
            case typelog::INFO:		    fox::info(Utils::MonoToString(formattedMessage));    break;
            case typelog::WARN:		    fox::warn(Utils::MonoToString(formattedMessage));    break;
            case typelog::ERROR:		fox::error(Utils::MonoToString(formattedMessage));   break;
            case typelog::CRITICAL:	fox::critical(Utils::MonoToString(formattedMessage));break;
            default:					fox::info(Utils::MonoToString(formattedMessage));   break;
        }
    }

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
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        FOX_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);
    }

    static uint64_t Entity_FindEntityByName(MonoString* name)
    {
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
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityToInstantiate);
        FOX_ASSERT(entity);

        return scene->CloneEntity(entity).GetUUID();
    }

    MonoString* NameComponent_GetName(uint64_t entityID)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        const auto& name = entity.get<EntityName>();
        return mono_string_new(mono_domain_get(), name.name.c_str());
    }

    void NameComponent_SetName(uint64_t entityID, MonoString* tag)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        const auto& name = entity.get<EntityName>();

        entity.get<EntityName>().name = mono_string_to_utf8(tag);
    }


    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        *outTranslation = entity.get<TransformComponent>().position;
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        entity.get<TransformComponent>().position = *translation;
    }

    void TransformComponent_GetRotation(uint64_t entityID, glm::vec3* outRotation)
    {
//        FOX_PROFILE_SCOPE();

        *outRotation = GetEntity(entityID).get<TransformComponent>().rotation;
    }

    void TransformComponent_SetRotation(uint64_t entityID, const glm::vec3* inRotation)
    {
//        FOX_PROFILE_SCOPE();

        GetEntity(entityID).get<TransformComponent>().rotation = *inRotation;
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

    // ------------------
    //  Sprite Renderer
    // ------------------

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

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
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
        Scene* scene = ScriptEngine::GetSceneContext();
        FOX_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        FOX_ASSERT(entity);

        auto& rb2d = entity.get<Rigidbody2D>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }

    bool Input_IsKeyPressed(KeyCode key)
    {
//        FOX_PROFILE_SCOPE();

        return Input::IsKeyPressed(key);
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

    #include <cxxabi.h>  // needed for abi::__cxa_demangle

    std::shared_ptr<char> cppDemangle(const char *abiName)
    {
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
        ([]()
        {
            std::string_view typeName = typeid(Component).name();
            auto demandleTypename = cppDemangle(typeName.data());
            std::string_view demandleTypenameView = demandleTypename.get();

            size_t pos = demandleTypenameView.find_last_of(':');
            std::string_view structName = demandleTypenameView.substr(pos + 1);
            std::string managedTypename = fox::format("Fox.%", structName);

            MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
            if (!managedType)
            {
                fox::error("Could not find component type %", managedTypename);
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

        RegisterComponent<EntityName>();
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

        FOX_ADD_INTERNAL_CALL(Entity_HasComponent);
        FOX_ADD_INTERNAL_CALL(Entity_FindEntityByName);
        FOX_ADD_INTERNAL_CALL(Entity_Instantiate);

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

        FOX_ADD_INTERNAL_CALL(Input_IsKeyPressed);
        FOX_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
        FOX_ADD_INTERNAL_CALL(Input_GetMousePosition);
    }

}
