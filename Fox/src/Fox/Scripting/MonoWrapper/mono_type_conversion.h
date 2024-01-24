#pragma once



#include "FMonoDomain.hpp"
#include "FMonoString.hpp"
#include "FMonoType.hpp"
#include "Scripting/MonoWrapper/FMonoString.hpp"
#include "mono_type_traits.h"

namespace fox::mono
{

template <typename T>
inline auto to_mono_arg(T& t)
{
	static_assert(is_mono_valuetype<T>::value, "Should not pass here for non-pod types");
	return std::addressof(t);
}
inline auto to_mono_arg(MonoObject* t)
{
	return t;
}

template <typename T>
struct convert_mono_type
{
	using cpp_type = T;
	using mono_unboxed_type = T;
	using mono_boxed_type = MonoObject*;

	static_assert(std::is_pod<mono_unboxed_type>::value, "Specialize convertor for non-pod types");

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		return obj;
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return obj;
	}

	static auto from_mono_boxed(const mono_boxed_type& obj) -> cpp_type
	{
		FMonoObject object(obj);
		const auto& type = object.GetType();
		const auto mono_sz = type.GetSizeof();
		const auto mono_align = type.GetAlignof();
		constexpr auto cpp_sz = sizeof(cpp_type);
		constexpr auto cpp_align = alignof(cpp_type);
		ignore(mono_align, mono_sz, cpp_sz, cpp_align);
		assert(mono_sz <= cpp_sz && mono_align <= cpp_align && "Different type layouts");
		void* ptr = mono_object_unbox(obj);
		return *reinterpret_cast<cpp_type*>(ptr);
	}
};

template <>
struct convert_mono_type<FMonoObject>
{
	using cpp_type = FMonoObject;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		return obj.GetInternalPtr();
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return cpp_type(obj);
	}

	static auto from_mono_boxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return cpp_type(obj);
	}
};

template <>
struct convert_mono_type<std::string>
{
	using cpp_type = std::string;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		const auto& domain = FMonoDomain::GetCurrentDomain();
		return FMonoString(domain, obj).GetInternalPtr();
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return FMonoString(FMonoObject(obj)).as_utf8();
	}

	static auto from_mono_boxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return FMonoString(FMonoObject(obj)).as_utf8();
	}
};

} // namespace fox::mono
