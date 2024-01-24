#pragma once

#include "FMonoException.hpp"
#include "FMonoMethod.h"
#include "FMonoObject.hpp"
#include "FMonoType.hpp"
#include "mono/metadata/object-forward.h"
#include "mono_type_conversion.h"

#include <functional>
#include <tuple>
#include <utility>
#include <vector>

namespace fox::mono
{

template <typename T>
auto is_compatible_type(const FMonoType& type) -> bool
{
	const auto& expected_name = type.GetFullname();
	return types::is_compatible_type<T>(expected_name);
}

template <typename Signature>
auto has_compatible_signature(const FMonoMethod& method) -> bool
{
	constexpr auto arity = function_traits<Signature>::arity;
	using return_type = typename function_traits<Signature>::return_type;
	using arg_types = typename function_traits<Signature>::arg_types_decayed;
	auto expected_rtype = method.get_return_type();
	auto expected_arg_types = method.get_param_types();

	bool compatible = arity == expected_arg_types.size();
	if(!compatible)
	{
		return false;
	}
	compatible &= is_compatible_type<return_type>(expected_rtype);
	if(!compatible)
	{
		// allow cpp return type to be void i.e ignoring it.
		if(!is_compatible_type<void>(expected_rtype))
		{
			return false;
		}
	}
	arg_types tuple;
	size_t idx = 0;
	for_each(tuple,
			 [&compatible, &idx, &expected_arg_types](const auto& arg)
			 {
				 ignore(arg);
				 auto expected_arg_type = expected_arg_types[idx];
				 using arg_type = decltype(arg);
				 compatible &= is_compatible_type<arg_type>(expected_arg_type);

				 idx++;
			 });

	return compatible;
}

template <typename T>
class mono_method_invoker;

template <typename... Args>
class mono_method_invoker<void(Args...)> : public FMonoMethod
{
public:
	void operator()(Args... args)
	{
		invoke(nullptr, std::forward<Args>(args)...);
	}

	void operator()(const FMonoObject& obj, Args... args)
	{
		invoke(&obj, std::forward<Args>(args)...);
	}

private:
	void invoke(const FMonoObject* obj, Args... args)
	{
		auto method = this->method_;
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->GetInternalPtr();

			method = mono_object_get_virtual_method(object, method);
		}
		auto tup =
			std::make_tuple(convert_mono_type<std::decay_t<Args>>::to_mono(std::forward<Args>(args))...);

		auto inv = [method, object](auto... args)
		{
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw FMonoThunkException(ex);
			}
		};

		apply(inv, tup);
	}

	template <typename Signature>
	friend auto make_method_invoker(const FMonoMethod&, bool) -> mono_method_invoker<Signature>;

	mono_method_invoker(const FMonoMethod& o)
		: FMonoMethod(o)
	{
	}
};

template <typename RetType, typename... Args>
class mono_method_invoker<RetType(Args...)> : public FMonoMethod
{
public:
	auto operator()(Args... args)
	{
		return invoke(nullptr, std::forward<Args>(args)...);
	}

	auto operator()(const FMonoObject& obj, Args... args)
	{
		return invoke(&obj, std::forward<Args>(args)...);
	}

private:
	auto invoke(const FMonoObject* obj, Args... args)
	{
		auto method = this->method_;
		MonoObject* object = nullptr;
		if(obj)
		{
			object = obj->GetInternalPtr();

			method = mono_object_get_virtual_method(object, method);
		}
		auto tup =
			std::make_tuple(convert_mono_type<std::decay_t<Args>>::to_mono(std::forward<Args>(args))...);
		auto inv = [method, object](auto... args)
		{
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			auto result = mono_runtime_invoke(method, object, argsv.data(), &ex);
			if(ex)
			{
				throw FMonoThunkException(ex);
			}

			return result;
		};

		auto result = fox::mono::apply(inv, tup);
		return convert_mono_type<std::decay_t<RetType>>::from_mono_boxed(std::move(result));
	}

	template <typename Signature>
	friend auto make_method_invoker(const FMonoMethod&, bool) -> mono_method_invoker<Signature>;

	mono_method_invoker(const FMonoMethod& o)
		: FMonoMethod(o)
	{
	}
};

template <typename Signature>
auto make_method_invoker(const FMonoMethod& method, bool check_signature = true)
	-> mono_method_invoker<Signature>
{
	if(check_signature && !has_compatible_signature<Signature>(method))
	{
		throw FMonoException("NATIVE::Method thunk requested with incompatible signature");
	}
	return mono_method_invoker<Signature>(method);
}

template <typename Signature>
auto make_method_invoker(const FMonoType& type, const std::string& name) -> mono_method_invoker<Signature>
{
	using arg_types = typename function_traits<Signature>::arg_types;
	arg_types tup;
	auto args_result = types::get_args_signature(tup);
	auto args = args_result.first;
	auto all_types_known = args_result.second;

	if(all_types_known)
	{
		auto func = type.GetMethod(name + "(" + args + ")");
		return make_method_invoker<Signature>(func);
	}
	else
	{
		constexpr auto arg_count = function_traits<Signature>::arity;
		auto func = type.GetMethod(name, arg_count);
		return make_method_invoker<Signature>(func);
	}
}

} // namespace fox::mono
