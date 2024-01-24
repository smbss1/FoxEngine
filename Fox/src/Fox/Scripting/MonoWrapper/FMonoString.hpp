#pragma once

#include "FMonoObject.hpp"

namespace fox::mono
{
    class FMonoDomain;

    class FMonoString : public FMonoObject
    {
    public:
        explicit FMonoString(const FMonoObject& obj);
        explicit FMonoString(const FMonoDomain& domain, const std::string& as_utf8);

        auto as_utf8() const -> std::string;
        auto as_utf16() const -> std::u16string;
        auto as_utf32() const -> std::u32string;
    };

} // namespace fox::mono