//
// Created by samuel on 11/10/22.
//

#ifndef FOXENGINE_UTILS_HPP
#define FOXENGINE_UTILS_HPP

//#include "ScriptEngine.hpp"

#include <filesystem>

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

    namespace Utils
    {
        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
        void PrintAssemblyTypes(MonoAssembly* assembly);
        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);
        std::string MonoToString(MonoString* monoString);
        bool HandleException(MonoObject* exception);
    }
}


#endif //FOXENGINE_UTILS_HPP
