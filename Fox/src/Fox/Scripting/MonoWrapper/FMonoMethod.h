#pragma once

#include "FMonoObject.hpp"
#include "mono_visibility.h"

namespace fox::mono
{

class FMonoObject;
class FMonoType;

class FMonoMethod
{
public:
	explicit FMonoMethod(MonoMethod* method);
	explicit FMonoMethod(const FMonoType& type, const std::string& name_with_args);
	explicit FMonoMethod(const FMonoType& type, const std::string& name, int argc);

	auto get_return_type() const -> FMonoType;
	auto get_param_types() const -> std::vector<FMonoType>;
	auto GetName() const -> const std::string&;
	auto GetFullname() const -> const std::string&;
	auto get_full_declname() const -> const std::string&;
	auto get_visibility() const -> visibility;
	auto is_static() const -> bool;
	auto is_virtual() const -> bool;

protected:
	void generate_meta();

	MonoMethod* method_ = nullptr;
	MonoMethodSignature* signature_ = nullptr;
	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};

} // namespace fox::mono
