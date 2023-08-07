//
// Created by samuel on 22/10/22.
//

#include "ScriptEngine.hpp"
#include "ScriptCache.hpp"
#include "Core/Assert.hpp"
#include "Utils.hpp"
#include "ScriptTypes.hpp"
#include "Asset/AssetManager.hpp"
#include "ScriptAsset.hpp"


#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/tokentype.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>

#include <unordered_map>

namespace fox
{
#define FOX_CACHE_CLASS(className, monoClass) ScriptCache::CacheClass(className, monoClass)
//			ScriptCache::CacheClassProperties(coreAssembly, managedClass);

    struct CacheStorage
    {
        std::unordered_map<uint32_t, ManagedClass> Classes;
        std::unordered_map<uint32_t, ManagedField> Fields;
        std::unordered_map<uint32_t, std::vector<ManagedMethod>> Methods;
        std::unordered_map<uint32_t, Ref<FieldStorage>> FieldStorageMap;
    };

    static CacheStorage* s_Cache = nullptr;
    void ScriptCache::Init()
    {
        FOX_CORE_ASSERT(!s_Cache, "Trying to initialize ScriptCache multiple times!");
        s_Cache = new CacheStorage();

        CacheCoreClasses();
    }

    void ScriptCache::CacheCoreClasses()
    {
#define CACHE_CORELIB_CLASS(name) FOX_CACHE_CLASS("System." name, mono_class_from_name(mono_get_corlib(), "System", name))

        CACHE_CORELIB_CLASS("Object");
        CACHE_CORELIB_CLASS("Boolean");
        CACHE_CORELIB_CLASS("SByte");
        CACHE_CORELIB_CLASS("Int16");
        CACHE_CORELIB_CLASS("Int32");
        CACHE_CORELIB_CLASS("Int64");
        CACHE_CORELIB_CLASS("Byte");
        CACHE_CORELIB_CLASS("UInt16");
        CACHE_CORELIB_CLASS("UInt32");
        CACHE_CORELIB_CLASS("UInt64");
        CACHE_CORELIB_CLASS("Single");
        CACHE_CORELIB_CLASS("Double");
        CACHE_CORELIB_CLASS("Char");
        CACHE_CORELIB_CLASS("String");

#define CACHE_FOX_CORE_CLASS(name) FOX_CACHE_CLASS("Fox." name, mono_class_from_name(ScriptEngine::GetCoreAssembly()->AssemblyImage, "Fox", name))

//        CACHE_HAZEL_CORE_CLASS("ShowInEditorAttribute");
//        CACHE_HAZEL_CORE_CLASS("HideFromEditorAttribute");
//        CACHE_HAZEL_CORE_CLASS("ClampValueAttribute");

        CACHE_FOX_CORE_CLASS("AssetHandle");
        CACHE_FOX_CORE_CLASS("Vector2");
        CACHE_FOX_CORE_CLASS("Vector3");
        CACHE_FOX_CORE_CLASS("Vector4");
        CACHE_FOX_CORE_CLASS("Entity");
        CACHE_FOX_CORE_CLASS("Prefab");
//        CACHE_HAZEL_CORE_CLASS("Material");
//        CACHE_HAZEL_CORE_CLASS("Collider");
//        CACHE_HAZEL_CORE_CLASS("BoxCollider");
//        CACHE_HAZEL_CORE_CLASS("SphereCollider");
//        CACHE_HAZEL_CORE_CLASS("CapsuleCollider");
//        CACHE_HAZEL_CORE_CLASS("MeshCollider");
//        CACHE_HAZEL_CORE_CLASS("PhysicsMaterial");
//        CACHE_HAZEL_CORE_CLASS("RaycastHit2D");
    }

    void ScriptCache::Shutdown()
    {
        ClearCache();

        delete s_Cache;
        s_Cache = nullptr;
    }

    void ScriptCache::ClearCache()
    {
        s_Cache->FieldStorageMap.clear();
        s_Cache->Fields.clear();
        s_Cache->Methods.clear();
        s_Cache->Classes.clear();
    }

    void ScriptCache::CacheClass(std::string_view className, MonoClass* monoClass)
    {
        MonoType* classType = mono_class_get_type(monoClass);
		ManagedClass managedClass;
		managedClass.FullName = className;
		managedClass.ID = Hash::FNVHash(managedClass.FullName);
        int alignment = 0;
		managedClass.Size = mono_type_size(classType, &alignment);
		managedClass.Class = monoClass;

        uint32_t classFlags = mono_class_get_flags(monoClass);
        managedClass.IsAbstract = classFlags & MONO_TYPE_ATTR_ABSTRACT;
        managedClass.IsStruct = mono_class_is_valuetype(monoClass);

		s_Cache->Classes[managedClass.ID] = managedClass;
		if (managedClass.FullName.find("Fox.") != std::string::npos)
		{
             auto coreAssembly = ScriptEngine::GetCoreAssembly();
			ScriptCache::CacheClassMethods(coreAssembly, managedClass);
            ScriptCache::CacheClassFields(coreAssembly, managedClass);
            for (auto fieldID : managedClass.Fields)
				s_Cache->FieldStorageMap[fieldID] = new_ref<FieldStorage>(&managedClass, &s_Cache->Fields[fieldID]);
        }
    }

    static void BuildClassMetadata(Ref<AssemblyInfo> assemblyInfo, MonoClass* monoClass)
    {
        const std::string& fullName = Utils::ResolveMonoClassName(monoClass);

        // C# adds a .<PrivateImplementationDetails> class for some reason?
        if (fullName.find("<PrivateImpl") != std::string::npos)
            return;

        ManagedClass managedClass;
        managedClass.FullName = fullName;
        managedClass.ID = Hash::FNVHash(managedClass.FullName);
        managedClass.Class = monoClass;

        uint32_t classFlags = mono_class_get_flags(monoClass);
        managedClass.IsAbstract = classFlags & MONO_TYPE_ATTR_ABSTRACT;
        managedClass.IsStruct = mono_class_is_valuetype(monoClass);

        MonoClass* parentClass = mono_class_get_parent(monoClass);
        if (parentClass != nullptr && parentClass != FOX_CACHED_CLASS_RAW("System.Object"))
        {
            std::string parentName = Utils::ResolveMonoClassName(parentClass);
            managedClass.ParentID = Hash::FNVHash(parentName);
        }

        assemblyInfo->Classes.push_back(managedClass.ID);
        s_Cache->Classes[managedClass.ID] = managedClass;
    }

    void ScriptCache::GenerateCacheForAssembly(Ref<AssemblyInfo> assemblyInfo)
    {
        const MonoTableInfo* tableInfo = mono_image_get_table_info(assemblyInfo->AssemblyImage, MONO_TABLE_TYPEDEF);
        int32_t tableRowCount = mono_table_info_get_rows(tableInfo);
        for (int32_t i = 1; i < tableRowCount; i++)
        {
            MonoClass* monoClass = mono_class_get(assemblyInfo->AssemblyImage, (i + 1) | MONO_TOKEN_TYPE_DEF);
            BuildClassMetadata(assemblyInfo, monoClass);
        }

        // Process fields and properties after all classes have been parsed.
        for (auto classID : assemblyInfo->Classes)
        {
            ManagedClass& managedClass = s_Cache->Classes.at(classID);

            CacheClassMethods(assemblyInfo, managedClass);

            scope<ManagedInstance> tempInstance = ManagedInstance::From(&managedClass);
            if (tempInstance == nullptr)
                continue;

            CacheClassFields(assemblyInfo, managedClass);
//            CacheClassProperties(AssemblyImage, managedClass);

            // Generate field storage
            if (!assemblyInfo->IsCoreAssembly)
            {
                for (auto fieldID : managedClass.Fields)
                    s_Cache->FieldStorageMap[fieldID] = new_ref<FieldStorage>(&managedClass, &s_Cache->Fields[fieldID]);
            }

            ManagedType classType = ManagedType::FromClass(&managedClass);
            if (classType.IsSubClassOf(FOX_CACHED_CLASS("Fox.Entity")))
                AssetManager::CreateNamedMemoryOnlyAsset<ScriptAsset>(managedClass.FullName, classID);
        }
    }

    ManagedClass* ScriptCache::GetManagedClassByName(const std::string_view& className)
    {
        if (s_Cache == nullptr)
            return nullptr;

        uint32_t classID = Hash::FNVHash(className);
        if (s_Cache->Classes.find(classID) == s_Cache->Classes.end())
            return nullptr;

        return &s_Cache->Classes[classID];
    }

    ManagedClass* ScriptCache::GetManagedClassByID(uint32_t classID)
    {
        if (s_Cache == nullptr)
            return nullptr;

        if (s_Cache->Classes.find(classID) == s_Cache->Classes.end())
            return nullptr;

        return &s_Cache->Classes[classID];
    }

    ManagedClass* ScriptCache::GetManagedClass(MonoClass* monoClass)
    {
        if (s_Cache == nullptr)
            return nullptr;

        if (monoClass == nullptr)
            return nullptr;

        return GetManagedClassByName(Utils::ResolveMonoClassName(monoClass));
    }

    ManagedClass* ScriptCache::GetMonoObjectClass(MonoObject* monoObject)
    {
//        FOX_PROFILE_FUNC();

        if (s_Cache == nullptr)
            return nullptr;

        MonoClass* objectClass = mono_object_get_class(monoObject);
        if (objectClass == nullptr)
            return nullptr;

        return GetManagedClassByName(Utils::ResolveMonoClassName(objectClass));
    }

    ManagedMethod* ScriptCache::GetManagedMethod(ManagedClass* managedClass, const std::string_view& name, uint32_t parameterCount, bool ignoreParent)
    {
//        FOX_PROFILE_FUNC();

        if (s_Cache == nullptr)
            return nullptr;

        if (managedClass == nullptr)
        {
            FOX_CORE_ERROR_TAG("ScriptEngine", "Attempting to get method % from a nullptr class!", name);
            return nullptr;
        }

        ManagedMethod* method = nullptr;

        uint32_t methodID = Hash::FNVHash(fox::format("%:%", managedClass->FullName, name));
        if (s_Cache->Methods.find(methodID) != s_Cache->Methods.end())
        {
            for (auto& methodCandiate : s_Cache->Methods.at(methodID))
            {
                if (methodCandiate.ParameterCount == parameterCount)
                {
                    method = &methodCandiate;
                    break;
                }
            }
        }

        if (method == nullptr && !ignoreParent && managedClass->ParentID != 0)
            method = GetManagedMethod(&s_Cache->Classes.at(managedClass->ParentID), name, parameterCount);

        if (method == nullptr)
            FOX_CORE_WARN_TAG("ScriptEngine", "Failed to find method with name: % and parameter count: % in class %", name, parameterCount, managedClass->FullName);
        return method;
    }

    ManagedField* ScriptCache::GetFieldByID(uint32_t fieldID)
    {
        if (s_Cache == nullptr)
            return nullptr;

        if (s_Cache->Fields.find(fieldID) == s_Cache->Fields.end())
            return nullptr;
        return &s_Cache->Fields.at(fieldID);
    }

    ManagedField* ScriptCache::GetFieldByName(const ManagedClass* managedClass, const std::string& fieldName)
    {
        uint32_t fieldID = Hash::FNVHash(managedClass->FullName + ":" + fieldName);
        if (s_Cache->Fields.find(fieldID) == s_Cache->Fields.end())
            return nullptr;

        return &s_Cache->Fields[fieldID];
    }

    Ref<FieldStorage> ScriptCache::GetFieldStorage(uint32_t fieldID)
    {
        if (s_Cache == nullptr)
            return nullptr;

        if (s_Cache->FieldStorageMap.find(fieldID) == s_Cache->FieldStorageMap.end())
            return nullptr;
        return s_Cache->FieldStorageMap.at(fieldID);
    }

    void ScriptCache::CacheClassMethods(Ref<AssemblyInfo> assemblyInfo, ManagedClass& managedClass)
    {
        MonoMethod* monoMethod = nullptr;
        void* methodPtr = 0;
        while ((monoMethod = mono_class_get_methods(managedClass.Class, &methodPtr)) != NULL)
        {
            MonoMethodSignature* sig = mono_method_signature(monoMethod);

            uint32_t flags = mono_method_get_flags(monoMethod, nullptr);
            char* fullName = mono_method_full_name(monoMethod, 0);

            ManagedMethod method;
            method.ID = Hash::FNVHash(fullName);
            method.FullName = fullName;
            method.ReturnType = ManagedType::FromType(mono_signature_get_return_type(sig));
            method.IsStatic = flags & MONO_METHOD_ATTR_STATIC;
            method.IsVirtual = flags & MONO_METHOD_ATTR_VIRTUAL;
            method.ParameterCount = mono_signature_get_param_count(sig);
            method.Method = monoMethod;

            /*if (method.ParameterCount > 0)
            {
                const char** parameterNames = new const char*[method.ParameterCount];
                mono_method_get_param_names(monoMethod, parameterNames);

                MonoType* parameterType = nullptr;
                void* parameterIter = 0;
                uint32_t parameterIndex = 0;
                while ((parameterType = mono_signature_get_params(sig, &parameterIter)) != NULL)
                {
                    const char* name = parameterNames[parameterIndex];

                    ManagedField parameter;
                    parameter.ID = Hash::GenerateFNVHash(name);
                    parameter.FullName = name;
                    parameter.Type = ManagedType::FromType(parameterType);
                    parameter.Attributes = 0;

                    int alignment = 0;
                    parameter.Size = mono_type_size(parameterType, &alignment);

                    method.Parameters[parameter.ID] = parameter;

                    parameterIndex++;
                }

                delete[] parameterNames;
            }*/

            s_Cache->Methods[method.ID].push_back(method);
            managedClass.Methods.push_back(method.ID);

            mono_free(fullName);
        }
    }

    void ScriptCache::CacheClassFields(Ref<AssemblyInfo> assemblyInfo, ManagedClass& managedClass)
    {
        MonoClassField* field = nullptr;
        void* fieldPtr = 0;
        while ((field = mono_class_get_fields(managedClass.Class, &fieldPtr)) != NULL)
        {
            ManagedField managedField;
            managedField.FullName = managedClass.FullName + ":" + mono_field_get_name(field);
            managedField.ID = Hash::FNVHash(managedField.FullName);

            // Properties have a backing field called <PropertyName>k__BackingField. We don't want to include those in the class fields list.
            if (managedField.FullName.find("k__BackingField") != std::string::npos)
                continue;

            managedField.Field = field;

            MonoType* monoType = mono_field_get_type(field);
            managedField.Type = ManagedType::FromType(monoType);

            if (!managedField.Type.IsValid())
                continue;

            managedField.Attributes = mono_custom_attrs_from_field(managedClass.Class, field);

//            if (managedField.HasAttribute("Fox.HideFromEditorAttribute"))
//                continue;

            uint32_t visibility = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
            switch (visibility)
            {
                case MONO_FIELD_ATTR_PUBLIC:
                    managedField.Flags |= (uint64_t)FieldFlag::Public;
                    break;
                case MONO_FIELD_ATTR_FAMILY:
                    managedField.Flags |= (uint64_t)FieldFlag::Protected;
                    break;
                case MONO_FIELD_ATTR_ASSEMBLY:
                    managedField.Flags |= (uint64_t)FieldFlag::Internal;
                    break;
                case MONO_FIELD_ATTR_PRIVATE:
                    managedField.Flags |= (uint64_t)FieldFlag::Private;
                    break;
            }

            managedField.DisplayName = managedField.FullName.substr(managedField.FullName.find_last_of(':') + 1);

//            if (managedField.HasAttribute("Fox.ShowInEditorAttribute"))
//            {
//                managedField.Flags &= ~(uint64_t)FieldFlag::Protected;
//                managedField.Flags &= ~(uint64_t)FieldFlag::Internal;
//                managedField.Flags &= ~(uint64_t)FieldFlag::Private;
//                managedField.Flags |= (uint64_t)FieldFlag::Public;
//
//                MonoObject* attrib = managedField.GetAttribute("Fox.ShowInEditorAttribute");
//                managedField.DisplayName = FOX_FIELD_VALUE("Fox.ShowInEditorAttribute", "DisplayName", std::string, attrib);
//                bool isReadOnly = FOX_FIELD_VALUE("Fox.ShowInEditorAttribute", "IsReadOnly", bool, attrib);
//
//                if (isReadOnly)
//                    managedField.Flags |= (uint64_t)FieldFlag::ReadOnly;
//            }

            int align;
            managedField.Size = mono_type_size(monoType, &align);
            managedClass.Size += managedField.Size;
            s_Cache->Fields[managedField.ID] = managedField;
            managedClass.Fields.push_back(managedField.ID);
        }
    }
}
