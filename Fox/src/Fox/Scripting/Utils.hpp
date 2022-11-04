//
// Created by samuel on 11/10/22.
//

#ifndef FOXENGINE_UTILS_HPP
#define FOXENGINE_UTILS_HPP

//#include "ScriptEngine.hpp"

#include <filesystem>
#include "mono/metadata/object.h"

extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;
typedef struct _MonoType MonoType;
typedef struct _MonoString MonoString;
}

namespace fox
{
    enum class ScriptFieldType;
    class ManagedType;
    class ManagedField;

    namespace Utils
    {
        class ValueWrapper;

        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPBD = false);
        void PrintAssemblyTypes(MonoAssembly* assembly);
        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);
        std::string MonoToString(MonoString* monoString);
        bool HandleException(MonoObject* exception);

        Utils::ValueWrapper GetFieldValue(MonoObject* monoObject, const ManagedField* field);
        MonoObject* GetFieldValueObject(MonoObject* monoObject, const ManagedField* field);

        void SetFieldValue(MonoObject* monoObject, const ManagedField* field, const Utils::ValueWrapper& value);

        Utils::ValueWrapper GetDefaultValueForType(const ManagedType& type);
        Utils::ValueWrapper MonoObjectToValue(MonoObject* obj, const ManagedType& type);
        MonoObject* ValueToMonoObject(const Utils::ValueWrapper& value, const ManagedType& type);

        std::string ResolveMonoClassName(MonoClass* monoClass);

        bool CheckMonoError(MonoError& error);
    }
}


#endif //FOXENGINE_UTILS_HPP
