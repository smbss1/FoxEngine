#include "FMonoField.h"
#include "FMonoDomain.hpp"
#include "FMonoException.hpp"


#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>

#include "Core/Assert.hpp"

namespace fox::mono
{

FMonoField::FMonoField(const FMonoType& type, const std::string& name)
	: field_(mono_class_get_field_from_name(type.GetInternalPtr(), name.c_str()))
	, name_(name)
{
	if(!field_)
	{
		throw FMonoException("NATIVE::Could not get field : " + name + " for class " + type.GetName());
	}
	const auto& domain = FMonoDomain::GetCurrentDomain();

	generate_meta();
	if(is_static())
	{
		owning_type_vtable_ = mono_class_vtable(domain.GetInternalPtr(), type.GetInternalPtr());
//		mono_runtime_class_init(owning_type_vtable_);
	}
}

void FMonoField::generate_meta()
{
	auto type = mono_field_get_type(field_);
	type_ = FMonoType(mono_class_from_mono_type(type));
	fullname_ = mono_field_full_name(field_);
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}

auto FMonoField::IsValueType() const -> bool
{
	return get_type().IsValueType();
}

auto FMonoField::GetName() const -> const std::string&
{
	return name_;
}
auto FMonoField::GetFullname() const -> const std::string&
{
	return fullname_;
}
auto FMonoField::get_full_declname() const -> const std::string&
{
	return full_declname_;
}
auto FMonoField::get_type() const -> const FMonoType&
{
	return type_;
}

auto FMonoField::get_visibility() const -> visibility
{
	uint32_t flags = mono_field_get_flags(field_) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

	if(flags == MONO_FIELD_ATTR_PRIVATE)
		return visibility::vis_private;
	else if(flags == MONO_FIELD_ATTR_FAM_AND_ASSEM)
		return visibility::vis_protected_internal;
	else if(flags == MONO_FIELD_ATTR_ASSEMBLY)
		return visibility::vis_internal;
	else if(flags == MONO_FIELD_ATTR_FAMILY)
		return visibility::vis_protected;
	else if(flags == MONO_FIELD_ATTR_PUBLIC)
		return visibility::vis_public;

	FOX_ASSERT(false);

	return visibility::vis_private;
}

auto FMonoField::is_static() const -> bool
{
	uint32_t flags = mono_field_get_flags(field_);

	return (flags & MONO_FIELD_ATTR_STATIC) != 0;
}
} // namespace fox::mono
