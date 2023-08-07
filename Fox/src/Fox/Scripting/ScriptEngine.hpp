//
// Created by samuel on 03/10/22.
//

#ifndef FOXENGINE_SCRIPTENGINE_HPP
#define FOXENGINE_SCRIPTENGINE_HPP

#include "Scene/Scene.hpp"
#include "Ecs/Entity.hpp"
#include "GCManager.hpp"
#include "ScriptTypes.hpp"
#include "ScriptCache.hpp"
#include "FieldStorage.hpp"

#include <filesystem>
#include <string>
#include <map>

extern "C" {
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoDomain MonoDomain;
}

//class BoxCollider2D;
namespace fox
{
    using ScriptEntityMap = std::vector<UUID>;

    struct ScriptComponent;

    struct Collision2DData
    {
        uint64_t OtherEntityID = 0;
    };

    class ScriptInstance : public ManagedInstance
    {
    public:
        ScriptInstance(ManagedClass* scriptClass, Entity entity);

        void InvokeOnCreate();
        void InvokeOnUpdate(float ts);
        void InvokeOnCollisionEnter2D(Collision2DData collisionInfo);
        void InvokeOnCollisionExit2D(Collision2DData collisionInfo);

        static Ref<ScriptInstance> ToRefFrom(ManagedClass* scriptClass, Entity entity);
        static Ref<ScriptInstance> ToRefFrom(const std::string_view& string, Entity entity);

    private:

        ManagedMethod* m_OnCreateMethod = nullptr;
        ManagedMethod* m_OnUpdateMethod = nullptr;

        ManagedMethod* m_OnCollisionEnter2DMethod = nullptr;
        ManagedMethod* m_OnCollisionExit2DMethod = nullptr;

        friend class ScriptEngine;
    };

    struct AssemblyInfo : public RefCounted
    {
        std::filesystem::path FilePath = "";
        MonoAssembly* Assembly = nullptr;
        MonoImage* AssemblyImage = nullptr;
        std::vector<uint32_t> Classes;
        bool IsCoreAssembly = false;
    };

    struct ScriptEngineSetting
    {
        std::filesystem::path CoreAssemblyPath;
        bool EnableDebugging;
        bool EnableProfiling;
    };

    class ScriptEngine
    {
    public:
        static void Init(const ScriptEngineSetting& config);
        static void Shutdown();

        static bool LoadAssembly();
        static bool LoadAppAssembly();
        static void ReloadAppDomain();
        static MonoDomain* GetAppDomain();
        static Ref<AssemblyInfo> GetCoreAssembly();
        static Ref<AssemblyInfo> GetAppAssembly();

        static void FindClassAndRegisterTypes();

        static void OnRuntimeStart(Scene* scene);
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);

        static void InitializeScriptEntity(Entity entity);
        static void ShutdownScriptEntity(Entity entity, bool erase = true);

        static void OnCreateEntity(Entity entity);
        static void OnEntityInstantiated(Entity entityDst, Entity entitySrc);

        static void OnUpdateEntity(Entity entity, Timestep ts);

        static void SetSceneContext(Scene* scene);
        static Scene* GetSceneContext();
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
        static const std::unordered_map<uint32_t, Ref<FieldStorage>>& GetEntityFields(UUID entityID);
        static Ref<FieldStorage> GetFieldStorage(Entity entity, uint32_t fieldID);

        static ManagedClass* GetEntityClass(const std::string& name);

        static MonoObject* GetManagedInstance(UUID uuid);

    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* CreateManagedObject(ManagedClass* managedClass);

        static void InitRuntimeObject(MonoClass* monoClass, MonoObject* object);
        static void InitRuntimeObject(MonoObject* object);

        static void LoadAssemblyClasses();
        static void CompileAppAssembly();

        friend class ManagedClass;
        friend class ScriptGlue;
        friend class ManagedClass;
        friend class ScriptCache;
        friend class FieldStorage;
        friend class ManagedInstance;
    };
}

#endif //FOXENGINE_SCRIPTENGINE_HPP
