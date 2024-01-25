//
// Created by samuel on 03/10/22.
//

#include "ScriptEngine.hpp"
#include "Core/UUID.hpp"
#include "ScriptGlue.hpp"
#include "ScriptCache.hpp"
#include "Marshal.hpp"
#include "Scripting/MonoWrapper/FMonoDomain.hpp"
#include "Scripting/MonoWrapper/FMonoJIT.hpp"
#include "Utils.hpp"
#include "FileWatch.hpp"
#include "Core/Application.hpp"
#include "Components/ScriptComponent.hpp"

#include "common.hpp"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

//#ifdef FOX_DEBUG
#include "Utils/FileSystem.hpp"
#include "Core/Project.hpp"
#include "Core/Any.hpp"
#include "Utils/Iterator.hpp"
#include "Asset/AssetManager.hpp"
#include "ScriptAsset.hpp"
//#endif // FOX_DEBUG

#include "MonoWrapper/FMonoJIT.hpp"

namespace fox
{
    struct ScriptEngineData
    {
        ScriptEngineSetting Setting;

        scope<mono::FMonoDomain> AppDomain;

        Ref<AssemblyInfo> CoreAssembly = nullptr;
        Ref<AssemblyInfo> AppAssembly = nullptr;

        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
        ScriptEntityMap ScriptEntities;

        scope<filewatch::FileWatch<fs::path>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

        // Runtime
        Scene* SceneContext = nullptr;
        std::unordered_map<UUID, std::unordered_map<uint32_t, Ref<FieldStorage>>> FieldMap;
    };

    static ScriptEngineData* s_Data = nullptr;

    static void OnAppAssemblyFileSystemEvent(const fs::path& path, const filewatch::Event change_type)
	{
        if (path.extension() != ".dll")
            return;

        FOX_INFO("Script changed, reloading... %", path.extension());
        if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;
            FOX_INFO("Script changed1, reloading...");

			Application::Get().SubmitToMainThread([]()
			{
				s_Data->AppAssemblyFileWatcher.reset();
				ScriptEngine::ReloadAppDomain();
			});
		}
	}

    void ScriptEngine::Init(const ScriptEngineSetting& setting)
    {
        s_Data = new ScriptEngineData();
        s_Data->Setting = setting;

        s_Data->AppAssembly = new_ref<AssemblyInfo>();
        s_Data->CoreAssembly = new_ref<AssemblyInfo>();

        InitMono();
        GCManager::Init();
        ScriptGlue::RegisterFunctions();

        if (!LoadAssembly())
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to build Hazel-ScriptCore! Most likely there were compile errors!");
            return;
        }
    }

    void ScriptEngine::Shutdown()
    {
        for (int i = 0; i < s_Data->ScriptEntities.size(); ++i)
        {
            UUID entityID = s_Data->ScriptEntities[i];
            ShutdownScriptEntity(s_Data->SceneContext->TryGetEntityByUUID(entityID));
        }
        s_Data->ScriptEntities.clear();

        s_Data->EntityInstances.clear();
        s_Data->FieldMap.clear();
        s_Data->AppAssemblyFileWatcher.reset();

        s_Data->SceneContext = nullptr;

        GCManager::Shutdown();

        ShutdownMono();
        delete s_Data;
    }

    void ScriptEngine::InitMono()
    {
        mono::FMonoJIT::SetAssembliesPath("mono/lib");

        // if (s_Data->Setting.EnableDebugging)
        // {
        //     static char* options[] = {
        //             "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
        //             "--soft-breakpoints"
        //         };
        //     mono_jit_parse_options(2, (char**)options);
        //     mono_debug_init(MONO_DEBUG_FORMAT_MONO);
        // }

        if (mono::FMonoJIT::Init("FoxJITRuntime", s_Data->Setting.EnableDebugging))
        {
            FOX_CORE_INFO_TAG("ScriptEngine", "Mono JIT initialized");
        }
        else
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to initialize Mono JIT!");
            return;
        }

        // Store the root domain pointer
        // s_Data->RootDomain = rootDomain;

        // if (s_Data->Setting.EnableDebugging)
        //     mono_debug_domain_create(s_Data->RootDomain);
        // mono_thread_set_main(mono_thread_current());
    }

    void ScriptEngine::ShutdownMono()
    {
//        FOX_PROFILE_SCOPE();
        ScriptCache::Shutdown();

        s_Data->AppDomain = nullptr;

        // mono_domain_set(mono_get_root_domain(), false);
        // if (s_Data->AppDomain)
        // {
        //     mono_domain_unload(s_Data->AppDomain);
        //     s_Data->AppDomain = nullptr;
        // }

        mono::FMonoJIT::Shutdown();
    }

    bool ScriptEngine::LoadAssembly()
    {
        if (!FileSystem::Exists(s_Data->Setting.CoreAssemblyPath))
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to load app assembly! Invalid filepath");
            FOX_CORE_ERROR_TAG("ScriptEngine", "Filepath = %", Project::ScriptModuleFilePath());
            return false;
        }

        // Create an App Domain
        s_Data->AppDomain = new_scope<mono::FMonoDomain>("FoxScriptRuntime");
        FOX_ASSERT(s_Data->AppDomain);
        // mono_domain_set(s_Data->AppDomain, true);
        FOX_CORE_INFO_TAG("ScriptEngine", "Load Core Assembly");

        s_Data->CoreAssembly->Assembly = Utils::LoadMonoAssembly(s_Data->Setting.CoreAssemblyPath, s_Data->Setting.EnableDebugging);
        s_Data->CoreAssembly->AssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly->Assembly);
        s_Data->CoreAssembly->Classes.clear();

        ScriptCache::Init();
        return true;
    }

    bool ScriptEngine::LoadAppAssembly()
    {
        if (!FileSystem::Exists(Project::ScriptModuleFilePath()))
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to load app assembly! Invalid filepath");
            FOX_CORE_ERROR_TAG("ScriptEngine", "Filepath = {}", Project::ScriptModuleFilePath());
            return false;
        }

        if (s_Data->AppAssembly->Assembly)
        {
            s_Data->AppAssembly->Assembly = nullptr;
            s_Data->AppAssembly->AssemblyImage = nullptr;

            if (!LoadAssembly())
                return false;
        }

        auto appAssembly = Utils::LoadMonoAssembly(Project::ScriptModuleFilePath(), s_Data->Setting.EnableDebugging);
        if (appAssembly == nullptr)
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to load app assembly!");
            return false;
        }

        s_Data->AppAssembly->Assembly = appAssembly;
        s_Data->AppAssembly->AssemblyImage = mono_assembly_get_image(s_Data->AppAssembly->Assembly);
        s_Data->AppAssembly->Classes.clear();

        s_Data->AppAssemblyFileWatcher = new_scope<filewatch::FileWatch<fs::path>>(Project::ScriptModuleFilePath(), OnAppAssemblyFileSystemEvent);
		s_Data->AssemblyReloadPending = false;

        LoadAssemblyClasses();
        FindClassAndRegisterTypes();
        return true;
    }

    void ScriptEngine::FindClassAndRegisterTypes()
    {
        ScriptGlue::ClearTypes();
        ScriptGlue::RegisterComponents();
    }

    void ScriptEngine::CompileAppAssembly()
    {
        FOX_CORE_INFO("Compile.... %", Project::ProjectDir());

#ifdef FOX_PLATFORM_WINDOWS
        std::string command = fox::format("cd .\\% && .\\build.bat", Project::ProjectDir());
#elif defined FOX_PLATFORM_LINUX
        std::string command = fox::format("cd ./% && ./build.sh", Project::ProjectDir());
#endif

        FOX_CORE_INFO("Command.... %", command);
        system(command.c_str());

        // Errors
        {
            FILE* errors = fopen((Project::ProjectDir() / "AssemblyBuildErrors.log").generic_string().c_str(), "r");

            char buffer[4096];
            if (errors != nullptr)
            {
                while (fgets(buffer, sizeof(buffer), errors))
                {
                    if (buffer)
                    {
                        size_t newLine = std::string_view(buffer).size() - 1;
                        buffer[newLine] = '\0';
                        FOX_CORE_ERROR(buffer);
                    }
                }

                fclose(errors);
            }
        }

        // Warnings
        {
            FILE* warns = fopen((Project::ProjectDir() / "AssemblyBuildWarnings.log").generic_string().c_str(), "r");

            char buffer[1024];
            if (warns != nullptr)
            {
                while (fgets(buffer, sizeof(buffer), warns))
                {
                    if (buffer)
                    {
                        size_t newLine = std::string_view(buffer).size() - 1;
                        buffer[newLine] = '\0';

                        if (strlen(buffer) > 2)
                            FOX_CORE_WARN(buffer);
                    }
                }

                fclose(warns);
            }
        }
        FOX_CORE_INFO("End Compile....");
    }

    void ScriptEngine::ReloadAppDomain()
    {
//        FOX_PROFILE_SCOPE();

        FOX_CORE_INFO_TAG("ScriptEngine", "Reloading %", Project::ScriptModuleFilePath());

        if (!FileSystem::Exists(Project::ScriptModuleFilePath()))
        {
            return;
        }

        // Cache old field values and destroy all previous script instances
        std::unordered_map<UUID, std::unordered_map<uint32_t, AnyValue>> oldFieldValues;

        auto view = s_Data->SceneContext->GetAllEntitiesWith<ScriptComponent>();
        for (const auto& enttEntity : view)
        {
            Entity entity { enttEntity, s_Data->SceneContext };
            UUID entityID = entity.GetUUID();
            auto& sc = entity.get<ScriptComponent>();

            ManagedClass* managedClass = ScriptCache::GetManagedClassByName(sc.ClassName);
            oldFieldValues[entityID] = std::unordered_map<uint32_t, AnyValue>();

            for (auto fieldID : managedClass->Fields)
            {
                Ref<FieldStorage> storage = s_Data->FieldMap[entityID][fieldID];
                if (!storage)
                    continue;

                const ManagedField* fieldInfo = storage->GetField();

                if (!fieldInfo->IsWritable())
                    continue;

                oldFieldValues[entityID][fieldID] = std::move(AnyValue::Copy(storage->GetValue()));
            }
            ShutdownScriptEntity(entity, false);
        }

//        system("call \"../vendor/premake/bin/premake5.exe\" --file=\"../Sandbox/premake5.lua\" vs2022");
        if (s_Data->AppDomain)
        {
            ScriptCache::Shutdown();

            // mono_domain_set(mono_get_root_domain(), false);
            // mono_domain_unload(s_Data->AppDomain);
            s_Data->AppDomain = nullptr;
        }

//        CompileAppAssembly();
        bool loaded = LoadAppAssembly();

        for (const auto& [entityID, fieldMap] : oldFieldValues)
        {
            Entity entity = s_Data->SceneContext->GetEntityByUUID(entityID);
            InitializeScriptEntity(entity);

            //const auto& sc = entity.get<ScriptComponent>();

            for (const auto& [fieldID, fieldValue] : fieldMap)
            {
                Ref<FieldStorage> storage = s_Data->FieldMap[entityID][fieldID];
                if (!storage)
                    continue;

                storage->SetValue(fieldValue.Data, fieldValue.Size);
            }
        }

        GCManager::CollectGarbage();
        FOX_CORE_INFO("Finished reload the C# Assembly");
    }

    MonoDomain* ScriptEngine::GetAppDomain()
    {
        return s_Data->AppDomain->GetInternalPtr();
    }

    Ref<AssemblyInfo> ScriptEngine::GetCoreAssembly()
    {
        return s_Data->CoreAssembly;
    }

    Ref<AssemblyInfo> ScriptEngine::GetAppAssembly()
    {
        return s_Data->AppAssembly;
    }

    void ScriptEngine::OnRuntimeStart(Scene* scene)
    {
        SetSceneContext(scene);
    }

    void ScriptEngine::OnRuntimeStop()
    {
        s_Data->SceneContext = nullptr;

        for (const auto& [entityID, instance] : s_Data->EntityInstances)
        {
            auto& entityFields = s_Data->FieldMap[entityID];
            for (auto [fieldID, storage] : entityFields)
            {
                storage->SetRuntimeInstance(-1);
            }
        }
        s_Data->EntityInstances.clear();
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
    {
        Ref<ScriptAsset> asset = AssetManager::GetAsset<ScriptAsset>(fullClassName);
        if (!asset)
            return false;

        return ScriptCache::GetManagedClassByID(asset->GetClassID()) != nullptr;
    }

    void ScriptEngine::InitializeScriptEntity(Entity entity)
    {
        if (!entity || !entity.has<ScriptComponent>())
            return;

        auto& sc = entity.get<ScriptComponent>();
        if (!EntityClassExists(sc.ClassName))
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Tried to initialize script entity with an invalid script!");
            return;
        }

        UUID entityID = entity.GetUUID();

        ManagedClass* managedClass = ScriptCache::GetManagedClassByName(sc.ClassName);
        auto filterRange = iter(managedClass->Fields)
                | map([](uint32_t fieldID) { return ScriptCache::GetFieldByID(fieldID); })
                | filter([](ManagedField* field) { return field->HasFlag(FieldFlag::Public); });
        while (const auto field = filterRange.next())
        {
            s_Data->FieldMap[entityID][(*field)->ID] = new_ref<FieldStorage>(managedClass, field.value());
        }
        s_Data->ScriptEntities.push_back(entityID);
    }

    void ScriptEngine::CopyScriptEntityData(Entity srcEntity, Entity dstEntity)
    {
        if (!srcEntity || !srcEntity.has<ScriptComponent>())
            return;
        if (!dstEntity || !dstEntity.has<ScriptComponent>())
            return;

        auto& srcEntitySc = srcEntity.get<ScriptComponent>();
        if (!EntityClassExists(srcEntitySc.ClassName))
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Tried to initialize script entity with an invalid script!");
            return;
        }

        auto& dstEntitySc = dstEntity.get<ScriptComponent>();
        if (!EntityClassExists(dstEntitySc.ClassName))
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Tried to initialize script entity with an invalid script!");
            return;
        }

        UUID srcEntityID = srcEntity.GetUUID();
        UUID dstEntityID = dstEntity.GetUUID();

        ManagedClass* srcEntityManagedClass = ScriptCache::GetManagedClassByName(srcEntitySc.ClassName);
        ManagedClass* dstEntityManagedClass = ScriptCache::GetManagedClassByName(dstEntitySc.ClassName);
        if (srcEntityManagedClass != dstEntityManagedClass)
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Tried to copy data from two different script!");
            return;
        }

        auto filterRange = iter(srcEntityManagedClass->Fields)
            | map([](uint32_t fieldID) { return ScriptCache::GetFieldByID(fieldID); })
            | filter([](ManagedField* field) { return field->HasFlag(FieldFlag::Public); });
        while (const auto field = filterRange.next())
        {
            s_Data->FieldMap[dstEntityID][(*field)->ID]->SetValue(s_Data->FieldMap[srcEntityID][(*field)->ID]->GetValue());
        }
    }

    void ScriptEngine::ShutdownScriptEntity(Entity entity, bool erase)
    {
//        FOX_PROFILE_FUNC();

        if (!entity || !entity.has<ScriptComponent>())
            return;

        auto& sc = entity.get<ScriptComponent>();
        UUID entityID = entity.GetUUID();

//        if (sc.IsRuntimeInitialized && sc.ManagedInstance != nullptr)
//            ShutdownRuntimeInstance(entity);

        s_Data->FieldMap.erase(entityID);
//        sc.ManagedInstance = nullptr;
//        sc.FieldIDs.clear();

        if (erase /*&& s_Data->ScriptEntities.find(sceneID) != s_Data->ScriptEntities.end()*/)
        {
            auto& scriptEntities = s_Data->ScriptEntities;
            scriptEntities.erase(std::remove(scriptEntities.begin(), scriptEntities.end(), entityID), scriptEntities.end());
        }
    }

    void ScriptEngine::OnCreateEntity(Entity entity)
    {
        const auto& sc = entity.get<ScriptComponent>();
        if (ScriptEngine::EntityClassExists(sc.ClassName))
        {
            UUID entityID = entity.GetUUID();

            Ref<ScriptInstance> instance = ScriptInstance::ToRefFrom(sc.ClassName, entity);
            s_Data->EntityInstances[entityID] = instance;

            auto& entityFields = s_Data->FieldMap[entityID];
            for (auto [fieldID, storage] : entityFields)
            {
                instance->SetFieldValue(storage->Field, storage->GetValue());
                storage->SetRuntimeInstance(instance->m_Handle);
            }
            instance->InvokeOnCreate();
        }
    }

    void ScriptEngine::OnEntityInstantiated(Entity entityDst, Entity entitySrc)
    {
//        FOX_PROFILE_FUNC();

        if (!entityDst.has<ScriptComponent>() || !entitySrc.has<ScriptComponent>())
            return;

        const auto& srcScriptComp = entitySrc.get<ScriptComponent>();
        auto& dstScriptComp = entityDst.get<ScriptComponent>();

        if (srcScriptComp.ClassName != dstScriptComp.ClassName)
        {
            const auto srcClass = GetEntityClass(srcScriptComp.ClassName);
            const auto dstClass = GetEntityClass(dstScriptComp.ClassName);
            FOX_CORE_WARN_TAG("ScriptEngine", "Attempting to duplicate instance of: {0} to an instance of: {1}", srcClass->FullName, dstClass->FullName);
            return;
        }

        ShutdownScriptEntity(entityDst);
        InitializeScriptEntity(entityDst);

        UUID targetEntityID = entityDst.GetUUID();
        UUID srcEntityID = entitySrc.GetUUID();

        Ref<ScriptInstance> instance = ScriptInstance::ToRefFrom(srcScriptComp.ClassName, entityDst);
        s_Data->EntityInstances[targetEntityID] = instance;

        const auto srcClass = GetEntityClass(srcScriptComp.ClassName);
        for (auto fieldID : srcClass->Fields)
        {
            if (s_Data->FieldMap.find(srcEntityID) == s_Data->FieldMap.end())
                break;

            if (s_Data->FieldMap.at(srcEntityID).find(fieldID) == s_Data->FieldMap.at(srcEntityID).end())
                continue;

            Ref<FieldStorage> storage = s_Data->FieldMap[targetEntityID][fieldID];

            storage->SetRuntimeInstance(instance->m_Handle);
            storage->CopyFrom(s_Data->FieldMap[srcEntityID][fieldID]);
        }
        instance->InvokeOnCreate();
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
    {
        UUID entityUUID = entity.GetUUID();
        FOX_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());

        Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
        instance->InvokeOnUpdate(ts);
    }

    void ScriptEngine::SetSceneContext(Scene* scene)
    {
        s_Data->SceneContext = scene;
    }

    Scene* ScriptEngine::GetSceneContext()
    {
        return s_Data->SceneContext;
    }

    Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
    {
        auto it = s_Data->EntityInstances.find(entityID);
        if (it == s_Data->EntityInstances.end())
            return nullptr;

        return it->second;
    }

    const std::unordered_map<uint32_t, Ref<FieldStorage>>& ScriptEngine::GetEntityFields(UUID entityID)
    {
        auto it = s_Data->FieldMap.find(entityID);
        if (it == s_Data->FieldMap.end())
            return {};

        return it->second;
    }

    Ref<FieldStorage> ScriptEngine::GetFieldStorage(Entity entity, uint32_t fieldID)
    {
        UUID entityID = entity.GetUUID();
        auto& fields = GetEntityFields(entityID);

        if (fields.empty()) {
            FOX_INFO("Is Empty: %", fields.empty());
            return nullptr;
        }

        if (fields.find(fieldID) == fields.end()) {
            FOX_INFO("Not Found");
            return nullptr;
        }

        return fields.at(fieldID);
    }

    ManagedClass* ScriptEngine::GetEntityClass(const std::string& fullClassName)
    {
        Ref<ScriptAsset> asset = AssetManager::GetAsset<ScriptAsset>(fullClassName);
        if (!asset)
            return nullptr;

        return ScriptCache::GetManagedClassByID(asset->GetClassID());
    }

    void ScriptEngine::LoadAssemblyClasses()
    {
        // NOTE: Maybe clear script files from asset manager
//        s_Data->EntityClasses.clear();
        GCManager::CollectGarbage();

        ScriptCache::GenerateCacheForAssembly(s_Data->AppAssembly);
    }

    MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
    {
        FOX_ASSERT(s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end());
        return s_Data->EntityInstances.at(uuid)->GetManagedObject();
    }

    MonoObject* ScriptEngine::CreateManagedObject(ManagedClass* managedClass)
    {
        MonoObject* monoObject = mono_object_new(s_Data->AppDomain->GetInternalPtr(), managedClass->Class);
        FOX_CORE_ASSERT(monoObject, "Failed to create MonoObject!");
        return monoObject;
    }

    void ScriptEngine::InitRuntimeObject(MonoClass* monoClass, MonoObject* object)
    {
        // Why doesn't mono/C# generate a default constructor if none exists... Mono requires one in order to properly initialize an object...
        // And for whatever bizzare reason it won't work if the parent class provides a default constructor...
        MonoMethod* defaultConstructor = mono_class_get_method_from_name(monoClass, ".ctor", 0);
        if (defaultConstructor == nullptr)
            return;
        InitRuntimeObject(object);
    }

    void ScriptEngine::InitRuntimeObject(MonoObject* object)
    {
        mono_runtime_object_init(object);
    }

    ScriptInstance::ScriptInstance(ManagedClass* managedClass, Entity entity) : ManagedInstance(managedClass)
    {
        m_OnCreateMethod = m_ManagedClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ManagedClass->GetMethod("OnUpdate", 1);

        ManagedClass* EntityClass = ScriptCache::GetManagedClassByName("Fox.Entity");

        m_OnCollisionEnter2DMethod = EntityClass->GetMethod("HandleOnCollisionEnter2D", 1);
        m_OnCollisionExit2DMethod = EntityClass->GetMethod("HandleOnCollisionExit2D", 1);

        // Call Entity constructor
        CallMethod(".ctor", entity.GetUUID());
    }

    void ScriptInstance::InvokeOnCreate()
    {
        CallMethod(m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)
    {
        CallMethod(m_OnUpdateMethod, ts);
    }

    void ScriptInstance::InvokeOnCollisionEnter2D(Collision2DData collisionInfo)
    {
        CallMethod(m_OnCollisionEnter2DMethod, collisionInfo);
    }

    void ScriptInstance::InvokeOnCollisionExit2D(Collision2DData collisionInfo)
    {
        CallMethod(m_OnCollisionExit2DMethod, collisionInfo);
    }

    Ref<ScriptInstance> ScriptInstance::ToRefFrom(ManagedClass* scriptClass, Entity entity)
    {
        return new_ref<ScriptInstance>(scriptClass, entity);
    }

    Ref<ScriptInstance> ScriptInstance::ToRefFrom(const std::string_view& fullClassName, Entity entity)
    {
        return ToRefFrom(ScriptCache::GetManagedClassByID(AssetManager::GetAsset<ScriptAsset>(fullClassName)->GetClassID()), entity);
    }
}
