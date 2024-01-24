#include "FMonoType.hpp"
#include "FMonoAssembly.hpp"
#include "FMonoException.hpp"

#include "FMonoDomain.hpp"
#include "FMonoField.h"
#include "FMonoMethod.h"
#include "FMonoObject.hpp"
#include "FMonoProperty.h"

#include <mono/metadata/debug-helpers.h>

namespace fox::mono
{

FMonoType::FMonoType() = default;

FMonoType::FMonoType(MonoImage* image, const std::string& name)
	: FMonoType(image, "", name)
{
}

FMonoType::FMonoType(MonoImage* image, const std::string& name_space, const std::string& name)
{
	class_ = mono_class_from_name(image, name_space.c_str(), name.c_str());

	if(!class_)
		throw FMonoException("NATIVE::Could not get class : " + name_space + "." + name);

	GenerateMeta();
}

FMonoType::FMonoType(MonoClass* cls)
{
	class_ = cls;
	if(!class_)
		throw FMonoException("NATIVE::Could not get class");

	GenerateMeta();
}
FMonoType::FMonoType(MonoType* type)
{
	class_ = mono_class_from_mono_type(type);
	if(!class_)
		throw FMonoException("NATIVE::Could not get class");

	GenerateMeta();
}
auto FMonoType::Valid() const -> bool
{
	return class_ != nullptr;
}

auto FMonoType::NewInstance() const -> FMonoObject
{
	const auto& domain = FMonoDomain::GetCurrentDomain();
	return FMonoObject(domain, *this);
}

auto FMonoType::GetMethod(const std::string& name_with_args) const -> FMonoMethod
{
	return FMonoMethod(*this, name_with_args);
}

auto FMonoType::GetMethod(const std::string& name, int argc) const -> FMonoMethod
{
	return FMonoMethod(*this, name, argc);
}

auto FMonoType::GetField(const std::string& name) const -> FMonoField
{
	return FMonoField(*this, name);
}

auto FMonoType::GetProperty(const std::string& name) const -> FMonoProperty
{
	return FMonoProperty(*this, name);
}

auto FMonoType::GetFields() const -> std::vector<FMonoField>
{
	void* iter = nullptr;
	auto field = mono_class_get_fields(class_, &iter);
	std::vector<FMonoField> fields;
	while(field)
	{
		std::string name = mono_field_get_name(field);

		fields.emplace_back(GetField(name));

		field = mono_class_get_fields(class_, &iter);
	}
	return fields;
}

auto FMonoType::GetProperties() const -> std::vector<FMonoProperty>
{
	void* iter = nullptr;
	auto prop = mono_class_get_properties(class_, &iter);
	std::vector<FMonoProperty> props;
	while(prop)
	{
		std::string name = mono_property_get_name(prop);

		props.emplace_back(GetProperty(name));

		prop = mono_class_get_properties(class_, &iter);
	}
	return props;
}

auto FMonoType::GetMethods() const -> std::vector<FMonoMethod>
{
	void* iter = nullptr;
	auto method = mono_class_get_methods(class_, &iter);
	std::vector<FMonoMethod> methods;

	while(method != nullptr)
	{
		auto sig = mono_method_signature(method);
		std::string signature = mono_signature_get_desc(sig, false);
		std::string name = mono_method_get_name(method);
		std::string fullname = name + "(" + signature + ")";
		methods.emplace_back(GetMethod(fullname));
		method = mono_class_get_methods(class_, &iter);
	}

	return methods;
}

auto FMonoType::HasBaseType() const -> bool
{
	return mono_class_get_parent(class_) != nullptr;
}

auto FMonoType::GetBaseType() const -> FMonoType
{
	auto base = mono_class_get_parent(class_);
	return FMonoType(base);
}

auto FMonoType::GetNestedTypes() const -> std::vector<FMonoType>
{
	void* iter = nullptr;
	auto nested = mono_class_get_nested_types(class_, &iter);
	std::vector<FMonoType> nested_clases;
	while(nested)
	{
		nested_clases.emplace_back(FMonoType(nested));
	}
	return nested_clases;
}

auto FMonoType::GetInternalPtr() const -> MonoClass*
{
	return class_;
}

void FMonoType::GenerateMeta()
{
	type_ = mono_class_get_type(class_);
	namespace_ = mono_class_get_namespace(class_);
	name_ = mono_class_get_name(class_);
	fullname_ = namespace_.empty() ? name_ : namespace_ + "." + name_;
	rank_ = mono_class_get_rank(class_);
	valuetype_ = !!mono_class_is_valuetype(class_);
	sizeof_ = std::uint32_t(mono_class_value_size(class_, &alignof_));
}

auto FMonoType::IsDerivedFrom(const FMonoType& type) const -> bool
{
	return mono_class_is_subclass_of(class_, type.GetInternalPtr(), false) != 0;
}
auto FMonoType::GetNamespace() const -> const std::string&
{
	return namespace_;
}
auto FMonoType::GetName() const -> const std::string&
{
	return name_;
}
auto FMonoType::GetFullname() const -> const std::string&
{
	return fullname_;
}
auto FMonoType::IsValueType() const -> bool
{
	return valuetype_;
}

auto FMonoType::GetRank() const -> int
{
	return rank_;
}

uint32_t FMonoType::GetSizeof() const
{
	return sizeof_;
}

uint32_t FMonoType::GetAlignof() const
{
	return alignof_;
}
} // namespace fox::mono