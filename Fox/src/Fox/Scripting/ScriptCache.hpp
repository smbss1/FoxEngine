//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_SCRIPTCACHE_HPP
#define FOXENGINE_SCRIPTCACHE_HPP

#include "Hash.hpp"
#include "FieldStorage.hpp"

#include <string>

extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;
}

#define FOX_CORE_CLASS(clazz) ScriptCache::GetManagedClassByName("Fox."#clazz)
#define FOX_CACHED_CLASS(clazz) ScriptCache::GetManagedClassByName(clazz)
#define FOX_CACHED_CLASS_RAW(clazz) ScriptCache::GetManagedClassByName(clazz)->Class
#define FOX_CACHED_METHOD(clazz, method, paramCount) ScriptCache::GetManagedMethod(ScriptCache::GetManagedClassByName(clazz), method, paramCount)
#define FOX_CACHED_FIELD(clazz, field) ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(clazz), field)
#define FOX_CACHED_FIELD_STORAGE(clazz, field) ScriptCache::GetFieldStorage(ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(clazz), field)->ID)
#define FOX_FIELD_VALUE(clazz, field, type, obj) ScriptCache::GetFieldStorage(ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(clazz), field)->ID)->GetValueForObject<type>(obj)
#define FOX_SCRIPT_CLASS_ID(name) Hash::FNVHash(name)


namespace fox
{
    class ManagedClass;
    class ManagedMethod;
    class ManagedField;
    struct AssemblyInfo;

    class ScriptCache
    {
    public:
        static void Init();
        static void Shutdown();

        static void CacheCoreClasses();
        static void ClearCache();

        static void GenerateCacheForAssembly(Ref<AssemblyInfo> assemblyInfo);

        static ManagedClass* GetManagedClassByName(const std::string& className);
        static ManagedClass* GetManagedClassByID(uint32_t classID);
        static ManagedClass* GetManagedClass(MonoClass* monoClass);
        static ManagedClass* GetMonoObjectClass(MonoObject* monoObject);
        static ManagedMethod* GetManagedMethod(ManagedClass* managedClass, const std::string& name, uint32_t parameterCount, bool ignoreParent = false);

        static ManagedField* GetFieldByID(uint32_t fieldID);
        static ManagedField* GetFieldByName(const ManagedClass* managedClass, const std::string& fieldName);
        static Ref<FieldStorage> GetFieldStorage(uint32_t fieldID);

    private:
        static void CacheClassMethods(Ref<AssemblyInfo> assemblyInfo, ManagedClass& managedClass);
        static void CacheClassFields(Ref<AssemblyInfo> assemblyInfo, ManagedClass& managedClass);

    };
}

#endif //FOXENGINE_SCRIPTCACHE_HPP
