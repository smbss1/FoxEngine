//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_SCRIPTTYPES_HPP
#define FOXENGINE_SCRIPTTYPES_HPP

#include "GCManager.hpp"
#include "Core/UUID.hpp"
#include "common.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Assert.hpp"
#include "ValueWrapper.hpp"
#include "ScriptCache.hpp"

#include <unordered_map>
#include <string>
#include <cstring>
#include <map>

extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;
typedef struct _MonoType MonoType;
}

namespace fox
{
    namespace Utils
    {
        class ValueWrapper;
    }

    enum class ScriptFieldType
    {
        None = 0,
        Float, Double,
        Bool, Char, Byte, Short, Int, Long,
        UByte, UShort, UInt, ULong,
        Vector2, Vector3, Vector4,
        String,
        Entity, Prefab, AssetHandle
    };

    class ManagedClass;
    struct ManagedType
    {
        MonoType* Type = nullptr;
        int TypeEncoding = 0;
        ManagedClass* TypeClass = nullptr;
        ScriptFieldType NativeType = ScriptFieldType::None;

        bool IsSubClassOf(ManagedClass* otherClass) const;
        bool CanAssignTo(ManagedClass* managedClass) const;
        bool IsVoid() const;
        bool IsArray() const;
        bool IsReferenceType() const;
        bool IsGeneric() const;
        bool IsPrimitiveType() const;
        ManagedType GetElementType() const;

        static ManagedType FromClass(ManagedClass* managedClass);
        static ManagedType FromClass(MonoClass* monoClass);
        static ManagedType FromType(MonoType* monoType);

        bool IsValid() const { return TypeEncoding != 0 && TypeClass != nullptr && NativeType != ScriptFieldType::None; }

        ManagedType() = default;
        ~ManagedType()
        {
            Type = nullptr;
            TypeClass = nullptr;
        }
    };

    enum class FieldFlag
    {
        None		= -1,
        ReadOnly	= BIT(0),
        Static		= BIT(1),
        Public		= BIT(2),
        Private		= BIT(3),
        Protected	= BIT(4),
        Internal	= BIT(5)
    };

    struct ManagedField
    {
        uint32_t ID = 0;
        std::string FullName = "";
        std::string DisplayName = "";
        ManagedType Type;
        uint64_t Flags = 0;
        uint32_t Size = 0;

        MonoClassField* Field = nullptr;
//        MonoProperty* Property = nullptr;
//        bool IsProperty;
//        Buffer DefaultValueBuffer;

        void* Attributes = nullptr;

        bool HasAttribute(const std::string& attributeName) const;
        MonoObject* GetAttribute(const std::string& attributeName);

        bool HasFlag(FieldFlag flag) const { return Flags & (uint64_t)flag; }

        bool IsWritable() const
        {
            return !HasFlag(FieldFlag::ReadOnly) && HasFlag(FieldFlag::Public);
        }

//        bool IsArray() const { return HasFlag(FieldFlag::IsArray); }


        ~ManagedField()
        {
            m_CachedAttributes.clear();
            Field = nullptr;
//            Property = nullptr;
        }

    private:
        std::unordered_map<std::string, MonoObject*> m_CachedAttributes;
    };

    struct ManagedMethod
    {
        uint32_t ID = 0;
        std::string FullName = "";
        ManagedType ReturnType;
        bool IsVirtual = false;
        bool IsStatic = false;
        uint32_t ParameterCount = 0;

        MonoMethod* Method = nullptr;

        ~ManagedMethod()
        {
            Method = nullptr;
        }
    };

    class ManagedInstance;

    class ManagedClass
    {
    public:
        uint32_t ID = 0;
        std::string FullName = "";
        std::vector<uint32_t> Fields;
        std::vector<uint32_t> Methods;
        uint32_t Size = 0;
        uint32_t ParentID = 0;
        MonoClass* Class = nullptr;

        // Will also be true if class is static
        bool IsAbstract = false;
        bool IsStruct = false;

    public:
        ManagedClass() = default;
        //ManagedClass(const std::string& classNamespace, const std::string& className, bool isCore = false);
        ~ManagedClass()
        {
            Class = nullptr;
        }

        template<typename... TArgs>
        scope<ManagedInstance>&& CreateInstance(TArgs&&... args);

        ManagedField* GetFieldByName(const std::string &name);
        ManagedMethod* GetMethod(const std::string_view& name, int parameterCount, bool ignoreParent = false);

        static ManagedClass* From(MonoObject* obj);

    private:
        MonoObject* CreateInstance();


    private:
        std::string m_ClassNamespace;
        std::string m_ClassName;

        friend class ScriptEngine;
    };

    class ManagedInstance : public RefCounted
    {
    public:
        ManagedInstance(ManagedClass* managedClass);
        ManagedInstance(GCHandle handle);
        ManagedInstance(MonoObject* obj, bool weakReference = false);
        ~ManagedInstance();

        MonoObject* GetManagedObject() const { return GCManager::GetReferencedObject(m_Handle); }
        bool IsValid() const { return GetManagedObject() != nullptr; }

        void InvokeMethod(ManagedMethod* method, void** params = nullptr, ManagedType returnType = ManagedType(), Utils::ValueWrapper* result = nullptr);

        template<typename... TArgs>
        void CallMethod(const std::string_view& methodName, TArgs&&... args)
        {
//            FOX_PROFILE_SCOPE(methodName.c_str());

            if (GetManagedObject() == nullptr)
            {
                FOX_CORE_WARN_TAG("ScriptEngine", "Attempting to call method % on an invalid instance!", methodName);
                return;
            }

            if (m_ManagedClass == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find ManagedClass!");
                return;
            }

            constexpr size_t argsCount = sizeof...(args);

            ManagedMethod* method = m_ManagedClass->GetMethod(methodName, argsCount);
            if (method == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find a C# method called % with % parameters", methodName, argsCount);
                return;
            }
            CallMethod(method, std::forward<TArgs>(args)...);
        }

        template<typename TReturn, typename... TArgs>
        TReturn CallMethodWithReturn(const std::string_view& methodName, TArgs&&... args)
        {
//            FOX_PROFILE_SCOPE(methodName.c_str());

            if (GetManagedObject() == nullptr)
            {
                FOX_CORE_WARN_TAG("ScriptEngine", "Attempting to call method % on an invalid object!", methodName);
                return;
            }

            if (m_ManagedClass == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find ManagedClass!");
                return;
            }

            constexpr size_t argsCount = sizeof...(args);
            ManagedMethod* method = m_ManagedClass->GetMethod(methodName, argsCount);
            if (method == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Failed to find a C# method called % with % parameters", methodName, argsCount);
                return;
            }

            // TODO
            /*if (!ScriptUtils::ValidateReturnType<TReturn>(method))
            {
                HZ_CORE_ERROR("[ScriptEngine]: Incorrect return type for method %", methodName);
                return TReturn();
            }*/

            Utils::ValueWrapper result;
            CallMethodWithReturn(method, result, std::forward<TArgs>(args)...);

            FOX_CORE_ASSERT(result.GetDataSize() > 0, "[ScriptEngine]: Unexpected result!");
            return result.Get<TReturn>();
        }

        template<typename T>
        T GetFieldValue(const ManagedField* field)
        {
            return GetFieldValue(field).template Get<T>();
        }

        Utils::ValueWrapper GetFieldValue(const ManagedField* field);
        void SetFieldValue(const ManagedField* field, const Utils::ValueWrapper& value) const;

        static scope<ManagedInstance> From(MonoObject* monoObject);

        template<typename... TConstructorArgs>
        static scope<ManagedInstance> From(const std::string_view& className, TConstructorArgs&&... args)
        {
            return From(ScriptCache::GetManagedClassByID(FOX_SCRIPT_CLASS_ID(className)), std::forward<TConstructorArgs>(args)...);
        }

        template<typename... TConstructorArgs>
        static scope<ManagedInstance> From(uint32_t classID, TConstructorArgs&&... args)
        {
            return From(ScriptCache::GetManagedClassByID(classID), std::forward<TConstructorArgs>(args)...);
        }

        template<typename... TConstructorArgs>
        static scope<ManagedInstance> From(ManagedClass* managedClass, TConstructorArgs&&... args)
        {
//            FOX_PROFILE_SCOPE_DYNAMIC(managedClass->FullName.c_str());

            if (managedClass == nullptr)
            {
                FOX_CORE_ERROR_TAG("ScriptEngine", "Attempting to create managed object with a null class!");
                return nullptr;
            }

            if (managedClass->IsAbstract)
                return nullptr;

            scope<ManagedInstance> instance = CreateManagedObject(managedClass);
            if (instance == nullptr)
                return nullptr;

            if (managedClass->IsStruct)
                return std::move(instance);

            instance->CallMethod(".ctor", std::forward<TConstructorArgs>(args)...);
            return std::move(instance);
        }

    protected:
        template<typename... TArgs>
        void CallMethod(ManagedMethod* method, TArgs&&... args)
        {
//            FOX_PROFILE_SCOPE(methodName.c_str());

            if (!method || !method->Method)
            {
                return;
            }

            constexpr size_t argsCount = sizeof...(args);
            if constexpr (argsCount > 0)
            {
                // NOTE: const void** -> void** BAD. Ugly hack because unpacking const parameters into void** apparantly isn't allowed (understandable)
                const void* data[] = { &args... };
                InvokeMethod(method, (void**)data);
            }
            else
            {
                InvokeMethod(method, nullptr);
            }
        }

        template<typename... TArgs>
        void CallMethodWithReturn(ManagedMethod* method, Utils::ValueWrapper* result, TArgs&&... args)
        {
//            FOX_PROFILE_SCOPE(methodName.c_str());

            constexpr size_t argsCount = sizeof...(args);
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
                InvokeMethod(method, (void**)data, method->ReturnType, result);
            }
            else
            {
                InvokeMethod(method, nullptr, method->ReturnType, result);
            }
        }

    private:
        static scope<ManagedInstance> CreateManagedObject(ManagedClass* managedClass);

    protected:
        ManagedClass* m_ManagedClass;
        GCHandle m_Handle;
    };


    template<typename... TArgs>
    scope<ManagedInstance>&& ManagedClass::CreateInstance(TArgs&&... args)
    {
        return ManagedInstance::From(this, std::forward<TArgs>(args)...);
    }

    namespace Utils
    {
        inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
        {
            switch (fieldType)
            {
                case ScriptFieldType::None:    return "None";
                case ScriptFieldType::Float:   return "Float";
                case ScriptFieldType::Double:  return "Double";
                case ScriptFieldType::Bool:    return "Bool";
                case ScriptFieldType::Char:    return "Char";
                case ScriptFieldType::Byte:    return "Byte";
                case ScriptFieldType::Short:   return "Short";
                case ScriptFieldType::Int:     return "Int";
                case ScriptFieldType::Long:    return "Long";
                case ScriptFieldType::UByte:   return "UByte";
                case ScriptFieldType::UShort:  return "UShort";
                case ScriptFieldType::UInt:    return "UInt";
                case ScriptFieldType::ULong:   return "ULong";
                case ScriptFieldType::String:  return "String";

                case ScriptFieldType::Vector2: return "Vector2";
                case ScriptFieldType::Vector3: return "Vector3";
                case ScriptFieldType::Vector4: return "Vector4";
                case ScriptFieldType::Entity:  return "Entity";
                case ScriptFieldType::Prefab:  return "Prefab";
                case ScriptFieldType::AssetHandle:  return "AssetHandle";
            }
            FOX_ASSERT(false, "Unknown ScriptFieldType");
            return "None";
        }

        inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
        {
            if (fieldType == "None")    return ScriptFieldType::None;
            if (fieldType == "Float")   return ScriptFieldType::Float;
            if (fieldType == "Double")  return ScriptFieldType::Double;
            if (fieldType == "Bool")    return ScriptFieldType::Bool;
            if (fieldType == "Char")    return ScriptFieldType::Char;
            if (fieldType == "Byte")    return ScriptFieldType::Byte;
            if (fieldType == "Short")   return ScriptFieldType::Short;
            if (fieldType == "Int")     return ScriptFieldType::Int;
            if (fieldType == "Long")    return ScriptFieldType::Long;
            if (fieldType == "UByte")   return ScriptFieldType::UByte;
            if (fieldType == "UShort")  return ScriptFieldType::UShort;
            if (fieldType == "UInt")    return ScriptFieldType::UInt;
            if (fieldType == "ULong")   return ScriptFieldType::ULong;
            if (fieldType == "String")   return ScriptFieldType::String;

            if (fieldType == "Vector2") return ScriptFieldType::Vector2;
            if (fieldType == "Vector3") return ScriptFieldType::Vector3;
            if (fieldType == "Vector4") return ScriptFieldType::Vector4;
            if (fieldType == "Entity")  return ScriptFieldType::Entity;
            if (fieldType == "Prefab")  return ScriptFieldType::Prefab;
            if (fieldType == "AssetHandle")  return ScriptFieldType::AssetHandle;

            FOX_ASSERT(false, "Unknown ScriptFieldType");
            return ScriptFieldType::None;
        }
    }
}

#endif //FOXENGINE_SCRIPTTYPES_HPP
