//
// Created by samuel on 22/10/22.
//

#include "ScriptTypes.hpp"
#include "ScriptEngine.hpp"
#include "Utils.hpp"

#include "mono/jit/jit.h"
#include "mono/metadata/object.h"

namespace fox
{
    static ScriptFieldType GetNativeType(int typeEncoding, ManagedClass* typeClass)
    {
        if (typeClass == nullptr)
            return ScriptFieldType::None;

        switch (typeEncoding)
        {
            case MONO_TYPE_VOID:		return ScriptFieldType::None;
            case MONO_TYPE_BOOLEAN:		return ScriptFieldType::Bool;
            case MONO_TYPE_CHAR:		return ScriptFieldType::Char;
            case MONO_TYPE_I1:			return ScriptFieldType::Byte;
            case MONO_TYPE_I2:			return ScriptFieldType::Short;
            case MONO_TYPE_I4:			return ScriptFieldType::Int;
            case MONO_TYPE_I8:			return ScriptFieldType::Long;
            case MONO_TYPE_U1:			return ScriptFieldType::UByte;
            case MONO_TYPE_U2:			return ScriptFieldType::UShort;
            case MONO_TYPE_U4:			return ScriptFieldType::UInt;
            case MONO_TYPE_U8:			return ScriptFieldType::ULong;
            case MONO_TYPE_R4:			return ScriptFieldType::Float;
            case MONO_TYPE_R8:			return ScriptFieldType::Double;
            case MONO_TYPE_STRING:		return ScriptFieldType::String;
            case MONO_TYPE_VALUETYPE:
            {
                if (typeClass == FOX_CORE_CLASS(AssetHandle))
                    return ScriptFieldType::AssetHandle;

                if (typeClass == FOX_CORE_CLASS(Vector2))
                    return ScriptFieldType::Vector2;

                if (typeClass == FOX_CORE_CLASS(Vector3))
                    return ScriptFieldType::Vector3;

                if (typeClass == FOX_CORE_CLASS(Vector4))
                    return ScriptFieldType::Vector4;

                break;
            }
            case MONO_TYPE_CLASS:
            {
                if (mono_class_is_assignable_from(typeClass->Class, FOX_CORE_CLASS(Entity)->Class))
                    return ScriptFieldType::Entity;

                if (typeClass == FOX_CORE_CLASS(Prefab))
                    return ScriptFieldType::Prefab;

                break;
            }
            case MONO_TYPE_SZARRAY:
            case MONO_TYPE_ARRAY:
            {
                MonoClass* elementClass = mono_class_get_element_class(typeClass->Class);
                if (elementClass == nullptr)
                    break;

                ManagedClass* managedElementClass = ScriptCache::GetManagedClass(elementClass);
                if (managedElementClass == nullptr)
                    break;

                int elementTypeEncoding = mono_type_get_type(mono_class_get_type(elementClass));
                return GetNativeType(elementTypeEncoding, managedElementClass);
            }
        }

        return ScriptFieldType::None;
    }

    bool ManagedType::IsSubClassOf(ManagedClass* otherClass) const
    {
        return mono_class_is_subclass_of(TypeClass->Class, otherClass->Class, false);
    }

    bool ManagedType::CanAssignTo(ManagedClass* managedClass) const
    {
        return mono_class_is_assignable_from(TypeClass->Class, managedClass->Class);
    }

    bool ManagedType::IsVoid() const { return TypeEncoding == MONO_TYPE_VOID; }

    bool ManagedType::IsArray() const
    {
        return TypeEncoding == MONO_TYPE_ARRAY || TypeEncoding == MONO_TYPE_SZARRAY;
    }

    bool ManagedType::IsReferenceType() const
    {
        return mono_type_is_reference(Type) || mono_type_is_byref(Type);
    }

    bool ManagedType::IsGeneric() const { return TypeEncoding == MONO_TYPE_GENERICINST; }

    bool ManagedType::IsPrimitiveType() const
    {
        switch (TypeEncoding)
        {
            case MONO_TYPE_VOID:
            case MONO_TYPE_STRING:
            case MONO_TYPE_VALUETYPE:
            case MONO_TYPE_CLASS:
            case MONO_TYPE_SZARRAY:
            case MONO_TYPE_ARRAY:
                return false;
        }

        return true;
    }

    ManagedType ManagedType::GetElementType() const
    {
        if (!IsArray())
            return ManagedType();

        return FromClass(mono_class_get_element_class(TypeClass->Class));
    }

    ManagedType ManagedType::FromClass(ManagedClass* managedClass)
    {
        MonoType* type = mono_class_get_type(managedClass->Class);
        int typeEncoding = mono_type_get_type(type);
        return { type, typeEncoding, managedClass, GetNativeType(typeEncoding, managedClass) };
    }

    ManagedType ManagedType::FromClass(MonoClass* monoClass)
    {
        MonoType* type = mono_class_get_type(monoClass);
        int typeEncoding = mono_type_get_type(type);
        ManagedClass* managedClass = ScriptCache::GetManagedClass(monoClass);
        return { type, typeEncoding, managedClass, GetNativeType(typeEncoding, managedClass) };
    }

    ManagedType ManagedType::FromType(MonoType* monoType)
    {
        return FromClass(mono_class_from_mono_type(monoType));
    }

    bool ManagedField::HasAttribute(const std::string& attributeName) const
    {
        const ManagedClass* attributeClass = ScriptCache::GetManagedClassByName(attributeName);
        if (attributeClass == nullptr)
            return false;

        if (Attributes == nullptr)
            return false;

        if (m_CachedAttributes.find(attributeName) != m_CachedAttributes.end())
            return true;

        return mono_custom_attrs_has_attr((MonoCustomAttrInfo*)Attributes, attributeClass->Class);
    }

    MonoObject* ManagedField::GetAttribute(const std::string& attributeName)
    {
        if (m_CachedAttributes.find(attributeName) != m_CachedAttributes.end())
            return m_CachedAttributes.at(attributeName);

        if (!HasAttribute(attributeName))
            return nullptr;

        MonoObject* attributeInstance = mono_custom_attrs_get_attr((MonoCustomAttrInfo*)Attributes, ScriptCache::GetManagedClassByName(attributeName)->Class);
        m_CachedAttributes[attributeName] = attributeInstance;
        return attributeInstance;
    }


    ManagedClass::ManagedClass(const std::string &classNamespace, const std::string &className, bool isCore)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        Class = mono_class_from_name(isCore ? ScriptEngine::GetCoreAssembly()->AssemblyImage : ScriptEngine::GetAppAssembly()->AssemblyImage,
                                           classNamespace.c_str(), className.c_str());
    }

    GCHandle ManagedClass::Instantiate()
    {
        return ScriptEngine::InstantiateClass(Class);
    }

    MonoObject *ManagedClass::CreateInstance()
    {
        MonoObject *obj = ScriptEngine::CreateManagedObject(Class);
        ScriptEngine::InitRuntimeObject(Class, obj);
        return obj;
    }

    ManagedField* ManagedClass::GetFieldByName(const std::string &name)
    {
        return ScriptCache::GetFieldByName(this, name);
    }

    ManagedMethod* ManagedClass::GetMethod(const std::string &name, int parameterCount, bool ignoreParent)
    {
        return ScriptCache::GetManagedMethod(this, name, parameterCount, ignoreParent);
    }

    void ManagedClass::InvokeMethod(MonoObject *instance, ManagedMethod* method, void **params, ManagedType returnType, Utils::ValueWrapper* result)
    {
        MonoObject *exception = nullptr;
        MonoObject *resultObj = mono_runtime_invoke(method->Method, instance, params, &exception);
        Utils::HandleException(exception);

        if (returnType.IsValid() && resultObj != nullptr)
            *result = Utils::MonoObjectToValue(resultObj, returnType);
    }
}
