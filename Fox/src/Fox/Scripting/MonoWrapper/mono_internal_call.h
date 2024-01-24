#pragma once



#include "FMonoAssembly.hpp"
#include "mono_type_conversion.h"


#include <mono/metadata/loader.h>


namespace fox::mono
{

template <typename F>
inline void add_internal_call(const std::string& name, F&& func)
{
	mono_add_internal_call(name.c_str(), reinterpret_cast<const void*>(func));
}

template <typename F, F& func>
struct mono_jit_internal_call_wrapper_void;

template <typename R, typename... Args, R (&func)(Args...)>
struct mono_jit_internal_call_wrapper_void<R(Args...), func>
{
	template <typename T>
	using args_t = convert_mono_type<std::decay_t<T>>;
	template <typename T>
	using boxed_t = typename args_t<T>::mono_unboxed_type;

	static void wrapper(boxed_t<Args>... args)
	{
		static_assert(std::is_void<std::decay_t<R>>::value,
					  "[ internal_vcall ] called with a function with return type. "
					  "Use [ internal_rcall ] instead.");
		func(args_t<Args>::from_mono_unboxed(std::move(args))...);
	}
};

template <typename F, F& func>
struct mono_jit_internal_call_wrapper;

template <typename R, typename... Args, R (&func)(Args...)>
struct mono_jit_internal_call_wrapper<R(Args...), func>
{
	using return_t = convert_mono_type<std::decay_t<R>>;
	using unboxed_t = typename return_t::mono_unboxed_type;

	template <typename T>
	using args_t = convert_mono_type<std::decay_t<T>>;
	template <typename T>
	using boxed_t = typename args_t<T>::mono_unboxed_type;

	static auto wrapper(boxed_t<Args>... args) -> unboxed_t
	{
		static_assert(!std::is_same<R, void>::value,
					  "[ internal_rcall ]' called with a function without a return type. "
					  "Use [ internal_vcall ] instead.");
		return return_t::to_mono(func(args_t<Args>::from_mono_unboxed(std::move(args))...));
	}
};

/*!
 * Wrap a non-static function for mono::add_internal_call, where automatic type
 * converstion is done through convert_mono_type. Add your own specialisation implementation
 * of this class to support more types.
 */

#define internal_rcall(func) &mono::mono_jit_internal_call_wrapper<decltype(func), func>::wrapper
#define internal_vcall(func) &mono::mono_jit_internal_call_wrapper_void<decltype(func), func>::wrapper

} // namespace fox::mono
