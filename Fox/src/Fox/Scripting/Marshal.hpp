//
// Created by samuel on 22/10/22.
//

#ifndef FOX_LEXER_MARSHAL_HPP
#define FOX_LEXER_MARSHAL_HPP

#include "ScriptTypes.hpp"

#include "mono/metadata/object.h"
#include <string>

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
    namespace Marshal
    {
        template<typename TValueType>
        TValueType Unbox(MonoObject* obj) { return *(TValueType*)mono_object_unbox(obj); }
        template<typename TValueType>
        TValueType UnboxAddress(MonoObject* obj) { return (TValueType*)mono_object_unbox(obj); }

        template<typename TValueType>
        MonoObject* BoxValue(const ManagedClass* valueClass, TValueType value);

        MonoString* EmptyString();
        std::string MonoStringToUTF8(MonoString* monoString);
        MonoString* UTF8StringToMono(const std::string& str);
    }
}

#include "Marshal.inl"
#endif //FOX_LEXER_MARSHAL_HPP
