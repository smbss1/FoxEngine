#pragma once
#include "FMonoField.h"

#include "FMonoDomain.hpp"
#include "FMonoObject.hpp"
#include "mono_type_conversion.h"
namespace fox::mono
{

template <typename T>
class mono_field_invoker : public FMonoField
{
public:
	void set_value(const T& val) const;

	void set_value(const FMonoObject& obj, const T& val) const;

	auto get_value() const -> T;

	auto get_value(const FMonoObject& obj) const -> T;

private:
	template <typename signature_t>
	friend auto make_field_invoker(const FMonoField&) -> mono_field_invoker<signature_t>;

	explicit mono_field_invoker(const FMonoField& field)
		: FMonoField(field)
	{
	}

	void set_value_impl(const FMonoObject* obj, const T& val) const;

	auto get_value_impl(const FMonoObject* obj) const -> T;
};

template <typename T>
void mono_field_invoker<T>::set_value(const T& val) const
{
	set_value_impl(nullptr, val);
}

template <typename T>
void mono_field_invoker<T>::set_value(const FMonoObject& object, const T& val) const
{
	set_value_impl(&object, val);
}

template <typename T>
void mono_field_invoker<T>::set_value_impl(const FMonoObject* object, const T& val) const
{
	assert(field_);

	auto mono_val = convert_mono_type<T>::to_mono(val);
	auto arg = to_mono_arg(mono_val);

	if(object)
	{
		auto obj = object->GetInternalPtr();
		assert(obj);
		mono_field_set_value(obj, field_, arg);
	}
	else
	{
		mono_field_static_set_value(owning_type_vtable_, field_, arg);
	}
}

template <typename T>
auto mono_field_invoker<T>::get_value() const -> T
{
	return get_value_impl(nullptr);
}

template <typename T>
auto mono_field_invoker<T>::get_value(const FMonoObject& object) const -> T
{
	return get_value_impl(&object);
}

template <typename T>
auto mono_field_invoker<T>::get_value_impl(const FMonoObject* object) const -> T
{
	T val{};
	assert(field_);
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if(!IsValueType())
	{
		arg = &refvalue;
	}
	if(object)
	{
		auto obj = object->GetInternalPtr();
		assert(obj);
		mono_field_get_value(obj, field_, arg);
	}
	else
	{
		mono_field_static_get_value(owning_type_vtable_, field_, arg);
	}

	if(!IsValueType())
	{
		val = convert_mono_type<T>::from_mono_boxed(refvalue);
	}
	return val;
}

template <typename T>
auto make_field_invoker(const FMonoField& field) -> mono_field_invoker<T>
{
	// const auto& expected_name = type.GetFullname();
	// bool compatible = types::is_compatible_type<T>(expected_name);
	return mono_field_invoker<T>(field);
}

template <typename T>
auto make_field_invoker(const FMonoType& type, const std::string& name) -> mono_field_invoker<T>
{
	auto field = type.GetField(name);
	return make_field_invoker<T>(field);
}
} // namespace fox::mono
