#pragma once



#include "FMonoType.hpp"
#include "mono_visibility.h"


#include <mono/metadata/object.h>


namespace fox::mono
{

class FMonoObject;

class FMonoProperty
{
public:
	explicit FMonoProperty(const FMonoType& type, const std::string& name);

	auto GetName() const -> const std::string&;

	auto GetFullname() const -> const std::string&;

	auto get_full_declname() const -> const std::string&;

	auto get_type() const -> const FMonoType&;

	auto get_get_method() const -> FMonoMethod;

	auto get_set_method() const -> FMonoMethod;

	auto get_visibility() const -> visibility;

	auto is_static() const -> bool;

	auto GetInternalPtr() const -> MonoProperty*;

private:
	void generate_meta();

	FMonoType type_;

	MonoProperty* property_ = nullptr;

	std::string name_;

	std::string fullname_;

	std::string full_declname_;
};

} // namespace fox::mono
