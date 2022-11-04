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
    struct ScriptComponent;

    struct Collision2DData
    {
        uint64_t OtherEntityID = 0;
    };

    class ScriptInstance : public RefCounted
    {
    public:
        ScriptInstance(ManagedClass* scriptClass, Entity entity);
        ~ScriptInstance();

        void InvokeOnCreate();
        void InvokeOnUpdate(float ts);
        void InvokeOnCollisionEnter2D(Collision2DData collisionInfo);
        void InvokeOnCollisionExit2D(Collision2DData collisionInfo);
        ManagedClass* GetManagedClass() { return m_ManagedClass; }

        template<typename T>
        T GetFieldValue(const ManagedField* field)
        {
            return GetFieldValue(field).template Get<T>();
        }

        Utils::ValueWrapper GetFieldValue(const ManagedField* field);
        void SetFieldValue(const ManagedField* field, const Utils::ValueWrapper& value);
        MonoObject* GetManagedObject() const { return GCManager::GetReferencedObject(m_Handle); }
    private:
        ManagedClass* m_ManagedClass;

        GCHandle m_Handle;

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
        static std::unordered_map<std::string, ManagedClass*> GetEntityClasses();

        static MonoObject* GetManagedInstance(UUID uuid);

        template<typename... TConstructorArgs>
        static MonoObject* CreateManagedObject(const std::string& className, TConstructorArgs&&... args)
        {
            return CreateManagedObject_Internal(ScriptCache::GetManagedClassByID(FOX_SCRIPT_CLASS_ID(className)), std::forward<TConstructorArgs>(args)...);
        }

        template<typename... TConstructorArgs>
        static MonoObject* CreateManagedObject(uint32_t classID, TConstructorArgs&&... args)
        {
            return CreateManagedObject_Internal(ScriptCache::GetManagedClassByID(classID), std::forward<TConstructorArgs>(args)...);
        }

    private:
        static void InitMono();
        static void ShutdownMono();

        static GCHandle InstantiateClass(MonoClass* monoClass);
        static MonoObject* CreateManagedObject(MonoClass* managedClass);

        template<typename... TConstructorArgs>
        static MonoObject* CreateManagedObject_Internal(ManagedClass* managedClass, TConstructorArgs&&... args)
        {
//            FOX_PROFILE_SCOPE_DYNAMIC(managedClass->FullName.c_str());

            if (managedClass == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Attempting to create managed object with a null class!");
                return nullptr;
            }

            if (managedClass->IsAbstract)
                return nullptr;

            MonoObject* obj = CreateManagedObject(managedClass->Class);
            bool str = managedClass->IsStruct;
            if (managedClass->IsStruct)
                return obj;

            constexpr size_t argsCount = sizeof...(args);
            ManagedMethod* ctor = managedClass->GetMethod(".ctor", argsCount);

            // Call the default ctor with 0 params
            InitRuntimeObject(obj);

            if constexpr (argsCount > 0)
            {
                if (ctor == nullptr)
                {
                    FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find a C# method called % with % parameters", managedClass->FullName, argsCount);
                    return obj;
                }

                const void* data[] = { &args... };
                managedClass->InvokeMethod(obj, ctor, (void**)data);
            }

            return obj;
        }


        static void InitRuntimeObject(MonoClass* monoClass, MonoObject* object);
        static void InitRuntimeObject(MonoObject* object);

        static void LoadAssemblyClasses();
        static void CompileAppAssembly();

        friend class ManagedClass;
        friend class ScriptGlue;
        friend class ManagedClass;
        friend class ScriptCache;
        friend class FieldStorage;
    };
}

#endif //FOXENGINE_SCRIPTENGINE_HPP
