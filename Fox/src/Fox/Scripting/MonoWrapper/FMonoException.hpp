#pragma once

#include <mono/metadata/reflection.h>
#include <stdexcept>

namespace fox::mono
{
    class FMonoException : public std::runtime_error
    {
        using runtime_error::runtime_error;
    };

    class FMonoThunkException : public FMonoException
    {
    public:
        explicit FMonoThunkException(MonoObject* ex);

        auto exception_typename() const -> const std::string&;

        auto message() const -> const std::string&;

        auto stacktrace() const -> const std::string&;

        struct FMonoExceptionInfo
        {
            std::string exception_typename;
            std::string message;
            std::string stacktrace;
        };

    private:
        explicit FMonoThunkException(const FMonoExceptionInfo& info);

        FMonoExceptionInfo info_;
    };

} // namespace fox::mono