#pragma once
#include "FMonoProperty.h"

#include "mono_method_invoker.h"

namespace fox::mono
{

template <typename T>
class mono_property_invoker : public FMonoProperty
{
public:
	void set_value(const T& val) const;

	void set_value(const FMonoObject& obj, const T& val) const;

	auto get_value() const -> T;

	auto get_value(const FMonoObject& obj) const -> T;

private:
	template <typename Signature>
	friend auto MakePropertyInvoker(const FMonoProperty&) -> mono_property_invoker<Signature>;

	explicit mono_property_invoker(const FMonoProperty& property)
		: FMonoProperty(property)
	{
	}
};

template <typename T>
void mono_property_invoker<T>::set_value(const T& val) const
{
	auto thunk = make_method_invoker<void(const T&)>(get_set_method());
	thunk(val);
}

template <typename T>
void mono_property_invoker<T>::set_value(const FMonoObject& object, const T& val) const
{
	auto thunk = make_method_invoker<void(const T&)>(get_set_method());
	thunk(object, val);
}

template <typename T>
auto mono_property_invoker<T>::get_value() const -> T
{
	auto thunk = make_method_invoker<T()>(get_get_method());
	return thunk();
}

template <typename T>
auto mono_property_invoker<T>::get_value(const FMonoObject& object) const -> T
{
	auto thunk = make_method_invoker<T()>(get_get_method());
	return thunk(object);
}

template <typename T>
auto MakePropertyInvoker(const FMonoProperty& property) -> mono_property_invoker<T>
{
	// const auto& expected_name = type.GetFullname();
	// bool compatible = types::is_compatible_type<T>(expected_name);
	return mono_property_invoker<T>(property);
}

template <typename T>
auto MakePropertyInvoker(const FMonoType& type, const std::string& name) -> mono_property_invoker<T>
{
	auto property = type.GetProperty(name);
	return MakePropertyInvoker<T>(property);
}
} // namespace fox::mono
