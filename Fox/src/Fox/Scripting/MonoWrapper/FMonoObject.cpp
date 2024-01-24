#include "FMonoObject.hpp"
#include "FMonoDomain.hpp"

namespace fox::mono
{
    FMonoObject::FMonoObject(MonoObject* object)
        : type_(mono_object_get_class(object))
        , object_(object)
    {
    }

    FMonoObject::FMonoObject(const FMonoDomain& domain, const FMonoType& type)
        : type_(type)
        , object_(mono_object_new(domain.GetInternalPtr(), type.GetInternalPtr()))
    {
        mono_runtime_object_init(object_);
    }
    auto FMonoObject::GetType() const -> const FMonoType&
    {
        return type_;
    }

    auto FMonoObject::Valid() const -> bool
    {
        return object_ != nullptr;
    }

    auto FMonoObject::GetInternalPtr() const -> MonoObject*
    {
        return object_;
    }

} // namespace fox::mono