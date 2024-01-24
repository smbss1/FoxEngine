#include "FMonoProperty.h"
#include "FMonoAssembly.hpp"
#include "FMonoException.hpp"
#include "FMonoMethod.h"


#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>


namespace fox::mono
{

FMonoProperty::FMonoProperty(const FMonoType& type, const std::string& name)
	: property_(mono_class_get_property_from_name(type.GetInternalPtr(), name.c_str()))
	, name_(name)
{
	if(!property_)
		throw FMonoException("NATIVE::Could not get property : " + name + " for class " + type.GetName());

	generate_meta();
}

auto FMonoProperty::GetInternalPtr() const -> MonoProperty*
{
	return property_;
}

auto FMonoProperty::GetName() const -> const std::string&
{
	return name_;
}

auto FMonoProperty::GetFullname() const -> const std::string&
{
	return fullname_;
}
auto FMonoProperty::get_full_declname() const -> const std::string&
{
	return full_declname_;
}

auto FMonoProperty::get_type() const -> const FMonoType&
{
	return type_;
}

auto FMonoProperty::get_get_method() const -> FMonoMethod
{
	auto method = mono_property_get_get_method(property_);
	return FMonoMethod(method);
}
auto FMonoProperty::get_set_method() const -> FMonoMethod
{
	auto method = mono_property_get_set_method(property_);
	return FMonoMethod(method);
}

auto FMonoProperty::get_visibility() const -> visibility
{
	auto getter_vis = visibility::vis_public;
	try
	{
		auto getter = get_get_method();
		getter_vis = getter.get_visibility();
	}
	catch(const FMonoException&)
	{
	}
	auto setter_vis = visibility::vis_public;
	try
	{
		auto setter = get_set_method();
		setter_vis = setter.get_visibility();
	}
	catch(const FMonoException&)
	{
	}
	if(int(getter_vis) < int(setter_vis))
	{
		return getter_vis;
	}

	return setter_vis;
}

auto FMonoProperty::is_static() const -> bool
{
	auto getter = get_get_method();
	return getter.is_static();
}

void FMonoProperty::generate_meta()
{
	auto GetMethod = get_get_method();
	type_ = FMonoType(GetMethod.get_return_type());
	fullname_ = name_;
	std::string storage = (is_static() ? " static " : " ");
	full_declname_ = to_string(get_visibility()) + storage + fullname_;
}
} // namespace fox::mono
