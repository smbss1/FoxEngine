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


namespace fox
{
    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define FOX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Fox.InternalCalls::" #Name, (void*) Name)

    static MonoObject* GetScriptInstance(UUID entityID)
    {
        return ScriptEngine::GetManagedInstance(entityID);
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

    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
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
            s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.has<Component>(); };
        }(), ...);
    }

    template<typename... Component>
    static void RegisterComponent(ComponentGroup<Component...>)
    {
        RegisterComponent<Component...>();
    }

    void ScriptGlue::RegisterComponents()
    {
        RegisterComponent(AllComponents{});
    }

    void ScriptGlue::RegisterFunctions()
    {
        FOX_ADD_INTERNAL_CALL(GetScriptInstance);

        FOX_ADD_INTERNAL_CALL(Entity_HasComponent);
        FOX_ADD_INTERNAL_CALL(Entity_FindEntityByName);

        FOX_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        FOX_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        FOX_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

        FOX_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }

}
