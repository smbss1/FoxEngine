//
// Created by samuel on 03/10/22.
//

#include "ScriptEngine.hpp"
#include "ScriptGlue.hpp"
#include "ScriptCache.hpp"
#include "Marshal.hpp"
#include "Utils.hpp"
#include "FileWatch.hpp"
#include "Core/Application.hpp"
#include "Components/ScriptComponent.hpp"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"

//#include "mono/metadata/threads.h"
//#include "mono/metadata/mono-debug.h"

//#ifdef FOX_DEBUG
#include "mono/metadata/debug-helpers.h"
//#endif // FOX_DEBUG

#include <fstream>

namespace fox
{
    static const char* s_CoreAssemblyPath = "Resources/Scripts/Fox-ScriptCore.dll";
    static const char* s_AppAssemblyPath = "SandboxProject/Assets/Scripts/Binaries/Sandbox.dll";

    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        Ref<AssemblyInfo> CoreAssembly = nullptr;
        Ref<AssemblyInfo> AppAssembly = nullptr;

        ManagedClass* EntityClass;

        std::unordered_map<std::string, ManagedClass*> EntityClasses;
        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

        scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

        // Runtime
        Scene* SceneContext = nullptr;
    };

    static ScriptEngineData* s_Data = nullptr;

    static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]()
			{
				s_Data->AppAssemblyFileWatcher.reset();
				ScriptEngine::ReloadAppDomain();
			});
		}
	}

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
        GCManager::Init();
        ScriptGlue::RegisterFunctions();

        LoadAssembly(s_CoreAssemblyPath);
        LoadAppAssembly(s_AppAssemblyPath);
        LoadAssemblyClasses();
        FindClassAndRegisterTypes();
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete s_Data;
    }

    void ScriptEngine::InitMono()
    {
        mono_set_assemblies_path("mono/lib");

//        static char* options[] =
//        {
//            "--soft-breakpoints",
//            "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n"
//        };
//        mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);
//        mono_debug_init(MONO_DEBUG_FORMAT_MONO);

        MonoDomain* rootDomain = mono_jit_init("FoxJITRuntime");
        FOX_ASSERT(rootDomain);

        // Store the root domain pointer
        s_Data->RootDomain = rootDomain;
//        mono_debug_domain_create(s_Data->RootDomain);
//        mono_thread_set_main(mono_thread_current());
    }

    void ScriptEngine::ShutdownMono()
    {
//        FOX_PROFILE_SCOPE();

		s_Data->EntityClasses.clear();
		s_Data->EntityInstances.clear();

		GCManager::Shutdown();

        mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
    }

    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
    {
        // Create an App Domain
        s_Data->AppDomain = mono_domain_create_appdomain("FoxScriptRuntime", nullptr);
        FOX_ASSERT(s_Data->AppDomain);
        mono_domain_set(s_Data->AppDomain, true);
//        mono_debug_domain_create(s_Data->AppDomain);

        // Move this maybe
        s_Data->CoreAssembly = new_ref<AssemblyInfo>();
        s_Data->CoreAssembly->Assembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssembly->AssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly->Assembly);
//        Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

        ScriptCache::Init();
//        MarshalUtils::Init();
    }

    void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
    {
        // Move this maybe
        s_Data->AppAssembly = new_ref<AssemblyInfo>();

        s_Data->AppAssembly->Assembly = Utils::LoadMonoAssembly(filepath);
        s_Data->AppAssembly->AssemblyImage = mono_assembly_get_image(s_Data->AppAssembly->Assembly);
        // Utils::PrintAssemblyTypes(s_Data->AppAssembly);

        s_Data->AppAssemblyFileWatcher = new_scope<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_Data->AssemblyReloadPending = false;
    }

    void ScriptEngine::FindClassAndRegisterTypes()
    {
        ScriptGlue::ClearTypes();
        ScriptGlue::RegisterComponents();

        // Retrieve and instantiate class
        s_Data->EntityClass = ScriptCache::GetManagedClassByName("Fox.Entity");
    }

    void ScriptEngine::CompileAppAssembly()
    {
        FOX_CORE_INFO("Compile....");
        std::string command = fox::format("dotnet msbuild %"
//                                          " -nologo"																	// no microsoft branding in console
//                                          " -noconlog"																// no console logs
//                                          " -f net45"																// build with NET 4.5
                                          //" -t:rebuild"																// rebuild the project
                                          " -m"																		// multiprocess build
//                                          " -flp1:Verbosity=minimal;logfile=AssemblyBuildErrors.log;errorsonly"		// dump errors in AssemblyBuildErrors.log file
//                                          " -flp2:Verbosity=minimal;logfile=AssemblyBuildWarnings.log;warningsonly"	// dump warnings in AssemblyBuildWarnings.log file
                                            , s_AppAssemblyPath);
        system(command.c_str());

        // Errors
        {
            FILE* errors = fopen("AssemblyBuildErrors.log", "r");

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
            FILE* warns = fopen("AssemblyBuildWarnings.log", "r");

            char buffer[1024];
            if (warns != nullptr)
            {
                while (fgets(buffer, sizeof(buffer), warns))
                {
                    if (buffer)
                    {
                        size_t newLine = std::string_view(buffer).size() - 1;
                        buffer[newLine] = '\0';
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

//        system("call \"../vendor/premake/bin/premake5.exe\" --file=\"../Sandbox/premake5.lua\" vs2022");
        FOX_CORE_INFO("Reloading the C# Assembly");
        if (s_Data->AppDomain)
        {
            ScriptCache::Shutdown();
//            MarshalUtils::Shutdown();

            mono_domain_set(mono_get_root_domain(), false);
            mono_domain_unload(s_Data->AppDomain);
            s_Data->AppDomain = nullptr;
        }

//        CompileAppAssembly();
        LoadAssembly(s_CoreAssemblyPath);
        LoadAppAssembly(s_AppAssemblyPath);
        LoadAssemblyClasses();
        FindClassAndRegisterTypes();

        GCManager::CollectGarbage();
        FOX_CORE_INFO("Finished reload the C# Assembly");
    }

    MonoDomain* ScriptEngine::GetAppDomain()
    {
        return s_Data->AppDomain;
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

        for (const auto& [id, instance] : s_Data->EntityInstances)
        {
            for (auto& fieldID : instance->m_ManagedClass->Fields)
            {
                auto storage = ScriptCache::GetFieldStorage(fieldID);
                storage->UnregisterInstance(instance->m_Handle);
            }
        }
        s_Data->EntityInstances.clear();
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
    {
        return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
    }

    void ScriptEngine::OnCreateEntity(Entity entity)
    {
        const auto& sc = entity.get<ScriptComponent>();
        if (ScriptEngine::EntityClassExists(sc.ClassName))
        {
            UUID entityID = entity.GetUUID();

            Ref<ScriptInstance> instance = new_ref<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
            s_Data->EntityInstances[entityID] = instance;

            ManagedClass* entityClass = instance->m_ManagedClass;
            const auto& fields = entityClass->Fields;
            for (const auto& id : fields)
            {
                auto storage = ScriptCache::GetFieldStorage(id);
                instance->SetFieldValue(storage->Field, storage->GetValue());
                storage->RegisterInstance(instance->m_Handle);
            }
            instance->InvokeOnCreate();
        }
    }

    void ScriptEngine::OnEntityInstantiated(Entity entityDst, Entity entitySrc)
    {
        const auto& sc = entityDst.get<ScriptComponent>();
        if (ScriptEngine::EntityClassExists(sc.ClassName))
        {
            UUID entityID = entityDst.GetUUID();

            Ref<ScriptInstance> instance = new_ref<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entityDst);
            s_Data->EntityInstances[entityID] = instance;

            // Copy field values from Src Entity
            for (auto& fieldID : instance->m_ManagedClass->Fields)
            {
                auto storage = ScriptCache::GetFieldStorage(fieldID);
                instance->SetFieldValue(storage->Field, storage->GetValue());
                storage->RegisterInstance(instance->m_Handle);
            }
            instance->InvokeOnCreate();
        }
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

    ManagedClass* ScriptEngine::GetEntityClass(const std::string& name)
    {
        if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
            return nullptr;

        return s_Data->EntityClasses.at(name);
    }

    std::unordered_map<std::string, ManagedClass*> ScriptEngine::GetEntityClasses()
    {
        return s_Data->EntityClasses;
    }

    void ScriptEngine::LoadAssemblyClasses()
    {
        GCManager::CollectGarbage();
        s_Data->EntityClasses.clear();
        ScriptCache::GenerateCacheForAssembly(s_Data->AppAssembly);

        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssembly->AssemblyImage, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssembly->AssemblyImage, "Fox", "Entity");

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssembly->AssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(s_Data->AppAssembly->AssemblyImage, cols[MONO_TYPEDEF_NAME]);
            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fox::format("%.%", nameSpace, className);
            else
                fullName = className;

            MonoClass* monoClass = mono_class_from_name(s_Data->AppAssembly->AssemblyImage, nameSpace, className);

            if (monoClass == nullptr || monoClass == entityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
            if (!isEntity)
                continue;

            ManagedClass* scriptClass = ScriptCache::GetManagedClassByName(fullName);
            s_Data->EntityClasses[fullName] = scriptClass;
        }
    }

    MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
    {
        FOX_ASSERT(s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end());
        return s_Data->EntityInstances.at(uuid)->GetManagedObject();
    }

    MonoObject* ScriptEngine::CreateManagedObject(MonoClass* managedClass)
    {
        MonoObject* monoObject = mono_object_new(s_Data->AppDomain, managedClass);
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

    GCHandle ScriptEngine::InstantiateClass(MonoClass* monoClass)
    {
//        FOX_PROFILE_SCOPE();

        MonoObject* object = CreateManagedObject(monoClass);
        if (object)
        {
            InitRuntimeObject(monoClass, object);
            return GCManager::CreateObjectReference(object, false);
        }

        return -1;
    }

    ScriptInstance::ScriptInstance(ManagedClass* managedClass, Entity entity)
        : m_ManagedClass(managedClass)
    {
        m_Handle = m_ManagedClass->Instantiate();

        m_OnCreateMethod = m_ManagedClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ManagedClass->GetMethod("OnUpdate", 1);

        m_OnCollisionEnter2DMethod = s_Data->EntityClass->GetMethod("HandleOnCollisionEnter2D", 1);
        m_OnCollisionExit2DMethod = s_Data->EntityClass->GetMethod("HandleOnCollisionExit2D", 1);

        // Call Entity constructor
        {
            UUID entityID = entity.GetUUID();
            m_ManagedClass->CallMethod(m_Handle, ".ctor", entityID);
        }
    }

    ScriptInstance::~ScriptInstance()
    {
        GCManager::ReleaseObjectReference(m_Handle);
    }

    void ScriptInstance::InvokeOnCreate()
    {
        if (m_OnCreateMethod)
            m_ManagedClass->InvokeMethod(GetManagedObject(), m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            m_ManagedClass->InvokeMethod(GetManagedObject(), m_OnUpdateMethod, &param);
        }
    }

    void ScriptInstance::InvokeOnCollisionEnter2D(Collision2DData collisionInfo)
    {
        void* params = &collisionInfo;
        m_ManagedClass->InvokeMethod(GetManagedObject(), m_OnCollisionEnter2DMethod, &params);
    }

    void ScriptInstance::InvokeOnCollisionExit2D(Collision2DData collisionInfo)
    {
        void* params = &collisionInfo;
        m_ManagedClass->InvokeMethod(GetManagedObject(), m_OnCollisionExit2DMethod, &params);
    }

    Utils::ValueWrapper ScriptInstance::GetFieldValue(const ManagedField* field)
    {
        return Utils::GetFieldValue(GetManagedObject(), field);
    }

    void ScriptInstance::SetFieldValue(const ManagedField* field, const Utils::ValueWrapper& value)
    {
        Utils::SetFieldValue(GetManagedObject(), field, value);
    }
}
