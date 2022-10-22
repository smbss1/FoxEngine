//
// Created by samuel on 22/10/22.
//

#ifndef FOX_LEXER_MARSHAL_INL
#define FOX_LEXER_MARSHAL_INL

#include "ScriptEngine.hpp"

namespace fox::Marshal
{
    template<typename TValueType>
    static MonoObject* BoxValue(const ManagedClass* valueClass, TValueType value)
    {
        return mono_value_box(ScriptEngine::GetAppDomain(), valueClass->Class, &value);
    }
}

#endif //FOX_LEXER_MARSHAL_INL
