#pragma once

#include "FMonoType.hpp"
#include <mono/metadata/object.h>

namespace fox::mono
{
class FMonoDomain;

class FMonoObject
{
public:
	explicit FMonoObject(MonoObject* object);
	explicit FMonoObject(const FMonoDomain& domain, const FMonoType& type);

	auto GetType() const -> const FMonoType&;

	auto Valid() const -> bool;

	auto GetInternalPtr() const -> MonoObject*;

protected:
	FMonoType type_;
	MonoObject* object_ = nullptr;
};

} // namespace fox::mono