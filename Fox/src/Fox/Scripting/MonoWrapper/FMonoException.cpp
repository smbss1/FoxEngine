#include "FMonoException.hpp"
#include "FMonoObject.hpp"
#include "mono_property_invoker.h"
#include "FMonoType.hpp"

namespace fox::mono
{

    auto get_exception_info(MonoObject* ex) -> FMonoThunkException::FMonoExceptionInfo
    {
        auto obj = FMonoObject(ex);
        const auto& type = obj.GetType();
        const auto& exception_typename = type.GetFullname();

        auto message_prop = type.GetProperty("Message");
        auto mutable_msg_prop = MakePropertyInvoker<std::string>(message_prop);

        auto stacktrace_prop = type.GetProperty("StackTrace");
        auto mutable_stack_prop = MakePropertyInvoker<std::string>(stacktrace_prop);

        auto message = mutable_msg_prop.get_value(obj);
        auto stacktrace = mutable_stack_prop.get_value(obj);
        return {exception_typename, message, stacktrace};
    }

    FMonoThunkException::FMonoThunkException(MonoObject* ex)
        : FMonoThunkException(get_exception_info(ex))
    {
    }

    auto FMonoThunkException::exception_typename() const -> const std::string&
    {
        return info_.exception_typename;
    }

    auto FMonoThunkException::message() const -> const std::string&
    {
        return info_.message;
    }

    auto FMonoThunkException::stacktrace() const -> const std::string&
    {
        return info_.stacktrace;
    }

    FMonoThunkException::FMonoThunkException(const FMonoExceptionInfo& info)
        : FMonoException(info.exception_typename + "(" + info.message + ")\n" + info.stacktrace)
        , info_(info)
    {
    }

} // namespace fox::mono