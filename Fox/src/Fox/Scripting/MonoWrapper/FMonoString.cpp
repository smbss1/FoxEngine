#include "FMonoString.hpp"
#include "FMonoDomain.hpp"
#include "Scripting/MonoWrapper/FMonoObject.hpp"

namespace fox::mono
{

    FMonoString::FMonoString(const FMonoObject& obj)
        : FMonoObject(obj)
    {
    }

    FMonoString::FMonoString(const FMonoDomain& domain, const std::string& str)
        : FMonoObject(reinterpret_cast<MonoObject*>(mono_string_new(domain.GetInternalPtr(), str.c_str())))
    {
    }

    auto FMonoString::as_utf8() const -> std::string
    {
        // TODO: This could be probably optimized by doing no additional
        // allocation though mono_string_chars and mono_string_length.
        MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

        auto raw_str = mono_string_to_utf8(mono_str);
        std::string str = reinterpret_cast<std::string::value_type*>(raw_str);
        mono_free(raw_str);
        return str;
    }

    auto FMonoString::as_utf16() const -> std::u16string
    {
        // TODO: This could be probably optimized by doing no additional
        // allocation though mono_string_chars and mono_string_length.
        MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

        auto raw_str = mono_string_to_utf16(mono_str);
        std::u16string str = reinterpret_cast<std::u16string::value_type*>(raw_str);
        mono_free(raw_str);
        return str;
    }

    auto FMonoString::as_utf32() const -> std::u32string
    {
        // TODO: This could be probably optimized by doing no additional
        // allocation though mono_string_chars and mono_string_length.
        MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

        auto raw_str = mono_string_to_utf32(mono_str);
        std::u32string str = reinterpret_cast<std::u32string::value_type*>(raw_str);
        mono_free(raw_str);
        return str;
    }

} // namespace mono