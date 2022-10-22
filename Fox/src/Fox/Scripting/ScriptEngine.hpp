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
        MonoObject* GetManagedObject() { return GCManager::GetReferencedObject(m_Handle); }
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

    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);
        static void LoadAppAssembly(const std::filesystem::path& filepath);
        static void ReloadAppDomain();
        static MonoDomain* GetAppDomain();
        static Ref<AssemblyInfo> GetCoreAssembly();
        static Ref<AssemblyInfo> GetAppAssembly();

        static void FindClassAndRegisterTypes();

        static void OnRuntimeStart(Scene* scene);
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);
        static void OnEntityInstantiated(Entity entityDst, Entity entitySrc);

        static void OnUpdateEntity(Entity entity, Timestep ts);

        static void SetSceneContext(Scene* scene);
        static Scene* GetSceneContext();
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);

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

            MonoObject* obj = CreateManagedObject(managedClass->Class);

            // Why doesn't mono/C# generate a default constructor if none exists... Mono requires one in order to properly initialize an object...
            // And for whatever bizzare reason it won't work if the parent class provides a default constructor...
            ManagedMethod* defaultConstructor = managedClass->GetMethod(".ctor", 0, true);
            if (defaultConstructor == nullptr)
                return obj;

            InitRuntimeObject(obj);

            constexpr size_t argsCount = sizeof...(args);
            ManagedMethod* ctor = managedClass->GetMethod(".ctor", argsCount);
            if (ctor == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find a C# method called % with % parameters", managedClass->FullName, argsCount);
                return nullptr;
            }

            if constexpr (argsCount > 0)
            {
                // TODO
                /*if (!ScriptUtils::ValidateParameterTypes<TArgs...>(method))
                {
                    HZ_CORE_ERROR("[ScriptEngine]: Attempting to call method % with invalid parameters!", methodName);
                    ScriptUtils::PrintInvalidParameters<TArgs...>(method);
                    return;
                }*/

                // NOTE: const void** -> void** BAD. Ugly hack because unpacking const parameters into void** apparantly isn't allowed (understandable)
                const void* data[] = { &args... };
                managedClass->InvokeMethod(obj, ctor, (void**)data);
            }
            else
            {
                managedClass->InvokeMethod(obj, ctor);
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
