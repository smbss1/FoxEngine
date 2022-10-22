/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** common.hpp
*/

#ifndef FOX_COMMON_HPP_
#define FOX_COMMON_HPP_

#include <vector>
#include <memory>
#include <cstring>
#include "Core/Core.hpp"
#include "Core/Ref.hpp"

namespace fox
{
	template<typename T>
	using scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr scope<T> new_scope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

//	template<typename T>
//	using Ref = std::shared_ptr<T>;
//
//	template<typename T, typename ... Args>
//	constexpr Ref<T> new_ref(Args&& ... args)
//	{
//		return std::make_shared<T>(std::forward<Args>(args)...);
//	}
}

#endif
