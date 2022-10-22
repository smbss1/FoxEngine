//
// Created by samuel on 22/10/22.
//

#include "Marshal.hpp"
#include "Utils.hpp"
#include "ScriptEngine.hpp"

#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace fox
{
    namespace Marshal
    {
        static MonoString* EmptyMonoString = nullptr;

        MonoString* EmptyString()
        {
            if (EmptyMonoString == nullptr)
            {
                EmptyMonoString = mono_string_empty(ScriptEngine::GetAppDomain());
            }
            return EmptyMonoString;
        }

        std::string MonoStringToUTF8(MonoString* monoString)
        {
            if (monoString == nullptr || mono_string_length(monoString) == 0)
                return "";

            MonoError error;
            char* utf8 = mono_string_to_utf8_checked(monoString, &error);
            if (Utils::CheckMonoError(error))
                return "";
            std::string result(utf8);
            mono_free(utf8);
            return result;
        }

        MonoString* UTF8StringToMono(const std::string& str)
        {
            return mono_string_new(ScriptEngine::GetAppDomain(), str.c_str());
        }
    }
}
