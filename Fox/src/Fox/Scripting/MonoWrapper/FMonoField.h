#pragma once



#include "FMonoType.hpp"
#include "mono_visibility.h"


#include <mono/metadata/object.h>


namespace fox::mono
{
class FMonoObject;

class FMonoField
{
public:
	explicit FMonoField(const FMonoType& type, const std::string& name);

	auto GetName() const -> const std::string&;

	auto GetFullname() const -> const std::string&;

	auto get_full_declname() const -> const std::string&;

	auto get_type() const -> const FMonoType&;

	auto get_visibility() const -> visibility;

	auto is_static() const -> bool;

protected:
	void generate_meta();
	auto IsValueType() const -> bool;

	FMonoType type_;

	MonoClassField* field_ = nullptr;

	MonoVTable* owning_type_vtable_ = nullptr;

	std::string name_;

	std::string fullname_;

	std::string full_declname_;
};

} // namespace fox::mono
