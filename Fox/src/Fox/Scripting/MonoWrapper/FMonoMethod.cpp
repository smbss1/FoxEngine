#include "FMonoMethod.h"
#include "FMonoException.hpp"
#include "FMonoType.hpp"


#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>

#include "Core/Assert.hpp"

namespace fox::mono
{

FMonoMethod::FMonoMethod(MonoMethod* method)
{
	method_ = method;
	if(!method_)
	{
		throw FMonoException("NATIVE::Could not create method");
	}
	generate_meta();
}

FMonoMethod::FMonoMethod(const FMonoType& type, const std::string& name_with_args)
{
	auto desc = mono_method_desc_new((":" + name_with_args).c_str(), 0);
	method_ = mono_method_desc_search_in_class(desc, type.GetInternalPtr());
	mono_method_desc_free(desc);

	if(!method_)
	{
		const auto& type_name = type.GetName();
		throw FMonoException("NATIVE::Could not create method : " + name_with_args + " for class " +
							 type_name);
	}
	generate_meta();
}

FMonoMethod::FMonoMethod(const FMonoType& type, const std::string& name, int argc)
{
	method_ = mono_class_get_method_from_name(type.GetInternalPtr(), name.c_str(), argc);

	if(!method_)
	{
		const auto& type_name = type.GetName();
		throw FMonoException("NATIVE::Could not create method : " + name + " for class " + type_name);
	}
	generate_meta();
}

void FMonoMethod::generate_meta()
{
	signature_ = mono_method_signature(method_);
	name_ = mono_method_get_name(method_);
	fullname_ = mono_method_full_name(method_, true);
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}

auto FMonoMethod::get_return_type() const -> FMonoType
{
	auto type = mono_signature_get_return_type(signature_);
	return FMonoType(type);
}

auto FMonoMethod::get_param_types() const -> std::vector<FMonoType>
{
	void* iter = nullptr;
	auto type = mono_signature_get_params(signature_, &iter);
	std::vector<FMonoType> params;
	while(type)
	{
		params.emplace_back(type);

		type = mono_signature_get_params(signature_, &iter);
	}

	return params;
}

auto FMonoMethod::GetName() const -> const std::string&
{
	return name_;
}

auto FMonoMethod::GetFullname() const -> const std::string&
{
	return fullname_;
}
auto FMonoMethod::get_full_declname() const -> const std::string&
{
	return full_declname_;
}
auto FMonoMethod::get_visibility() const -> visibility
{

	uint32_t flags = mono_method_get_flags(method_, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

	if(flags == MONO_METHOD_ATTR_PRIVATE)
		return visibility::vis_private;
	else if(flags == MONO_METHOD_ATTR_FAM_AND_ASSEM)
		return visibility::vis_protected_internal;
	else if(flags == MONO_METHOD_ATTR_ASSEM)
		return visibility::vis_internal;
	else if(flags == MONO_METHOD_ATTR_FAMILY)
		return visibility::vis_protected;
	else if(flags == MONO_METHOD_ATTR_PUBLIC)
		return visibility::vis_public;

	FOX_ASSERT(false);

	return visibility::vis_private;
}

auto FMonoMethod::is_static() const -> bool
{
	uint32_t flags = mono_method_get_flags(method_, nullptr);
	return (flags & MONO_METHOD_ATTR_STATIC) != 0;
}
auto FMonoMethod::is_virtual() const -> bool
{
	uint32_t flags = mono_method_get_flags(method_, nullptr);
	return (flags & MONO_METHOD_ATTR_VIRTUAL) != 0;
}
} // namespace fox::mono
