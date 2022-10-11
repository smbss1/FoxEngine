//
// Created by samuel on 03/10/22.
//

#include "Components/ScriptComponent.hpp"

#include "ScriptEngine.hpp"
#include "ScriptGlue.hpp"
#include "Utils.hpp"

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

        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;

        MonoAssembly* AppAssembly = nullptr;
        MonoImage* AppAssemblyImage = nullptr;

        ref<ScriptClass> EntityClass;

        std::unordered_map<std::string, ref<ScriptClass>> EntityClasses;
        std::unordered_map<UUID, ref<ScriptInstance>> EntityInstances;
        std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

        // Runtime
        Scene* SceneContext = nullptr;
    };

    static ScriptEngineData* s_Data = nullptr;

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

#if 0
        // Create an object (and call constructor)
        MonoObject* instance = s_Data->EntityClass.Instantiate();

        // Call method
        MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
        s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);

        // Call method with param
        MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

        int value = 5;
        void* params[1] = {
            &value,
        };

        s_Data->EntityClass.InvokeMethod(instance, printIntFunc, params);

        MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);
        int value2 = 508;
        void* params[2] =
        {
            &value,
            &value2
        };
        s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);

        MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");
        MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
        void* stringParam = monoString;
        s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);

        FOX_ASSERT(false);
#endif
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
		s_Data->EntityScriptFields.clear();
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
        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
//        Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
    }

    void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
    {
        // Move this maybe
        s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
        // Utils::PrintAssemblyTypes(s_Data->AppAssembly);
    }

    void ScriptEngine::FindClassAndRegisterTypes()
    {
        ScriptGlue::ClearTypes();
        ScriptGlue::RegisterComponents();

        // Retrieve and instantiate class
        s_Data->EntityClass = new_ref<ScriptClass>("Fox", "Entity", true);
    }

    void ScriptEngine::CompileAppAssembly()
    {
        fox::info("Compile....");
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
                        fox::error(buffer);
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
                        fox::warn(buffer);
                    }
                }

                fclose(warns);
            }
        }
        fox::info("End Compile....");
    }

    void ScriptEngine::ReloadAppDomain()
    {
//        FOX_PROFILE_SCOPE();

//        system("call \"../vendor/premake/bin/premake5.exe\" --file=\"../Sandbox/premake5.lua\" vs2022");
        fox::info("Reloading the C# Assembly");
        if (s_Data->AppDomain)
        {
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
        fox::info("Finished reload the C# Assembly");
    }


    void ScriptEngine::OnRuntimeStart(Scene* scene)
    {
        s_Data->SceneContext = scene;
    }

    void ScriptEngine::OnRuntimeStop()
    {
        s_Data->SceneContext = nullptr;
        s_Data->EntityInstances.clear();
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
    {
        return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
    }

    ref<ScriptInstance> ScriptEngine::CreateEntityInstance(Entity entity)
    {
        ref<ScriptInstance> instance = new_ref<ScriptInstance>(s_Data->EntityClass, entity);
        s_Data->EntityInstances[entity.GetUUID()] = instance;
        return instance;
    }

    ref<ScriptInstance> ScriptEngine::CreateEntityInstance(Entity entity, const ScriptComponent& component)
    {
        ref<ScriptInstance> instance = new_ref<ScriptInstance>(s_Data->EntityClasses[component.ClassName], entity);
        s_Data->EntityInstances[entity.GetUUID()] = instance;
        return instance;
    }

    void ScriptEngine::OnCreateEntity(Entity entity)
    {
        const auto& sc = entity.get<ScriptComponent>();
        if (ScriptEngine::EntityClassExists(sc.ClassName))
        {
            UUID entityID = entity.GetUUID();

            ref<ScriptInstance> instance = new_ref<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
            s_Data->EntityInstances[entityID] = instance;

            // Copy field values
            if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end())
            {
                const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(entityID);
                for (const auto& [name, fieldInstance] : fieldMap)
                {
                    if (fieldInstance.Field.Type == ScriptFieldType::Entity)
                    {
                        // get the id ref of the entity
                        UUID entityId = fieldInstance.GetValue<UUID>();
                        if (entityId <= 0)
                            continue;

                        // TODO: Caching the instance ? (better perf ?)
                        // if id is ok
                        // get the entity instance in the scene
                        Entity entityRef = s_Data->SceneContext->GetEntityByUUID(entityId);
                        // create a new instance C#
                        ref<ScriptInstance> obj = CreateEntityInstance(entityRef);
                        // Set into the field
                        instance->SetFieldValueInternal(name, obj->GetManagedObject());
                    }
                    else
                    {
                        instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
                    }
                }
            }
            instance->InvokeOnCreate();
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
    {
        UUID entityUUID = entity.GetUUID();
        FOX_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());

        ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
        instance->InvokeOnUpdate(ts);
    }

    Scene* ScriptEngine::GetSceneContext()
    {
        return s_Data->SceneContext;
    }

    ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
    {
        auto it = s_Data->EntityInstances.find(entityID);
        if (it == s_Data->EntityInstances.end())
            return nullptr;

        return it->second;
    }

    ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
    {
        if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
            return nullptr;

        return s_Data->EntityClasses.at(name);
    }

    Entity ScriptEngine::GetEntityInstanceFromMonoObject(MonoObject* obj)
    {
        for (auto script : s_Data->EntityInstances)
        {
            if (script.second->GetManagedObject() == obj)
                return s_Data->SceneContext->GetEntityByUUID(script.first);
        }

        return Entity();
    }


    std::unordered_map<std::string, ref<ScriptClass>> ScriptEngine::GetEntityClasses()
    {
        return s_Data->EntityClasses;
    }

    ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
    {
        FOX_ASSERT(entity);

        UUID entityID = entity.GetUUID();
        return s_Data->EntityScriptFields[entityID];
    }

    void ScriptEngine::LoadAssemblyClasses()
    {
        GCManager::CollectGarbage();
        s_Data->EntityClasses.clear();

        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Fox", "Entity");

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fox::format("%.%", nameSpace, className);
            else
                fullName = className;

            MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);

            if (monoClass == nullptr || monoClass == entityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
            if (!isEntity)
                continue;

            ref<ScriptClass> scriptClass = new_ref<ScriptClass>(nameSpace, className);
            s_Data->EntityClasses[fullName] = scriptClass;


            // This routine is an iterator routine for retrieving the fields in a class.
            // You must pass a gpointer that points to zero and is treated as an opaque handle
            // to iterate over all of the elements. When no more values are available, the return value is NULL.

            int fieldCount = mono_class_num_fields(monoClass);
            fox::warn("% has % fields:", className, fieldCount);
            void* iterator = nullptr;
            while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
            {
                const char* fieldName = mono_field_get_name(field);
                uint32_t flags = mono_field_get_flags(field);
                if (flags & FIELD_ATTRIBUTE_PUBLIC)
                {
                    MonoType* type = mono_field_get_type(field);
                    ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
                    fox::warn("  % (%)", fieldName, Utils::ScriptFieldTypeToString(fieldType));

                    scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
                }
            }
        }
    }

    MonoImage* ScriptEngine::GetCoreAssemblyImage()
    {
        return s_Data->CoreAssemblyImage;
    }

    MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
    {
        FOX_ASSERT(s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end());
        return s_Data->EntityInstances.at(uuid)->GetManagedObject();
    }

    GCHandle ScriptEngine::InstantiateClass(MonoClass* monoClass)
    {
//        FOX_PROFILE_SCOPE();

        MonoObject* object = mono_object_new(s_Data->AppDomain, monoClass);
        if (object)
        {
            mono_runtime_object_init(object);
            return GCManager::CreateObjectReference(object, false);
        }

        return -1;
    }

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    GCHandle ScriptClass::Instantiate()
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    {
        MonoObject* exception = nullptr;
        MonoObject* result = mono_runtime_invoke(method, instance, params, &exception);
        if (Utils::HandleException(exception))
            return nullptr;
        return result;
    }

    ScriptInstance::ScriptInstance(ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        m_Handle = scriptClass->Instantiate();

        m_Constructor = s_Data->EntityClass->GetMethod(".ctor", 1);
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

        m_OnCollisionEnter2DMethod = s_Data->EntityClass->GetMethod("HandleOnCollisionEnter2D", 1);
        m_OnCollisionExit2DMethod = s_Data->EntityClass->GetMethod("HandleOnCollisionExit2D", 1);

        // Call Entity constructor
        {
            UUID entityID = entity.GetUUID();
            void* param = &entityID;
            m_ScriptClass->InvokeMethod(GetManagedObject(), m_Constructor, &param);
        }
    }

    ScriptInstance::~ScriptInstance()
    {
        GCManager::ReleaseObjectReference(m_Handle);
    }

    void ScriptInstance::InvokeOnCreate()
    {
        if (m_OnCreateMethod)
            m_ScriptClass->InvokeMethod(GetManagedObject(), m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            m_ScriptClass->InvokeMethod(GetManagedObject(), m_OnUpdateMethod, &param);
        }
    }

    void ScriptInstance::InvokeOnCollisionEnter2D(Collision2DData collisionInfo)
    {
        void* params = &collisionInfo;
        m_ScriptClass->InvokeMethod(GetManagedObject(), m_OnCollisionEnter2DMethod, &params);
    }

    void ScriptInstance::InvokeOnCollisionExit2D(Collision2DData collisionInfo)
    {
        void* params = &collisionInfo;
        m_ScriptClass->InvokeMethod(GetManagedObject(), m_OnCollisionExit2DMethod, &params);
    }

    bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
    {
        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_get_value(GetManagedObject(), field.ClassField, buffer);
        return true;
    }

    MonoObject* ScriptInstance::GetFieldObjectValueInternal(const std::string& name)
    {
//        FOX_PROFILE_SCOPE();

        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return nullptr;

        const ScriptField& field = it->second;
        return mono_field_get_value_object(s_Data->AppDomain, field.ClassField, GetManagedObject());
    }

    bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
    {
        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        void* valueStore = (void*) value;

        if (field.Type == ScriptFieldType::String)
        {
            MonoString* monoStr = mono_string_new(s_Data->AppDomain, (const char*)value);
            valueStore = monoStr;
        }
        mono_field_set_value(GetManagedObject(), field.ClassField, valueStore);
        return true;
    }
}
