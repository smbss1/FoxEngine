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
#include "Utils/bitsets.hpp"

using EntityId = std::size_t;
using CompId = EntityId;
using CompSignature = bitset;


constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}

inline std::uint32_t CreateEventId(char const* s)
{
	return fnv1a_32(s, std::strlen(s));
}

namespace fox
{
	const std::uint32_t EcsPrefab = 51;
	const std::uint32_t EcsDisable = 50;
	namespace ecs
	{
		const std::uint32_t OnAdd = "fox::ecs::OnAdd"_hash;
		const std::uint32_t OnRemove = "fox::ecs::OnRemove"_hash;
		const std::uint32_t OnSet = "fox::ecs::OnSet"_hash;

        const std::uint32_t OnSceneEnable = "fox::ecs::OnEnableScene"_hash;
        const std::uint32_t OnSceneDisable = "fox::ecs::OnSceneDisable"_hash;

        const std::uint32_t OnLoad = "fox::ecs::OnLoad"_hash;
		const std::uint32_t PostLoad = "fox::ecs::PostLoad"_hash;
		const std::uint32_t PreFixUpdate = "fox::ecs::PreFixUpdate"_hash;
		const std::uint32_t PostFixUpdate = "fox::ecs::PostFixUpdate"_hash;

		const std::uint32_t PreUpdate = "fox::ecs::PreUpdate"_hash;
		const std::uint32_t OnUpdate = "fox::ecs::OnUpdate"_hash;
		const std::uint32_t OnValidate = "fox::ecs::OnValidate"_hash;
		const std::uint32_t PostUpdate = "fox::ecs::PostUpdate"_hash;
		const std::uint32_t PreStore = "fox::ecs::PreStore"_hash;
		const std::uint32_t OnStore = "fox::ecs::OnStore"_hash;

        const std::uint32_t OnAddScript = "fox::ecs::OnAddScript"_hash;
	}

	namespace game
	{
		const std::uint32_t OnStart = "fox::game::OnStart"_hash;
	}

	template<typename T>
	using scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr scope<T> new_scope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr ref<T> new_ref(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#endif