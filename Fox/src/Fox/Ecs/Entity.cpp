/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Entity.cpp
*/

#include "Ecs/Entity.hpp"
#include "Components/IDComponent.hpp"
#include "Components/EntityName.hpp"
#include "Components/Transform.hpp"

namespace fox
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

    UUID Entity::GetUUID() const { return get<IDComponent>().ID; }
    const std::string& Entity::GetName() const { return get<EntityName>().name; }
    TransformComponent& Entity::GetTransform() const { return get<TransformComponent>(); }

    void Entity::SetParentUUID(UUID parent) { get<HierarchyComponent>().ParentID = parent; }
    UUID Entity::GetParentUUID() const { return get<HierarchyComponent>().ParentID; }
    Entity Entity::GetParent() const
    {
        return m_Scene->TryGetEntityByUUID(GetParentUUID());
    }

    void Entity::SetParent(Entity parent)
    {
        Entity currentParent = GetParent();
        if (currentParent == parent)
            return;

        // If changing parent, remove child from existing parent
        if (currentParent)
            currentParent.RemoveChild(*this);

        // Setting to null is okay
        SetParentUUID(parent.GetUUID());

        if (parent)
        {
            auto& parentChildren = parent.Children();
            UUID uuid = GetUUID();
            if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
                parentChildren.emplace_back(GetUUID());
        }
    }

    std::vector<UUID>& Entity::Children() { return get<HierarchyComponent>().Children; }
    const std::vector<UUID>& Entity::Children() const { return get<HierarchyComponent>().Children; }

    bool Entity::HasChildren() const { return !Children().empty(); }

    bool Entity::RemoveChild(Entity child)
    {
        UUID childId = child.GetUUID();
        std::vector<UUID>& children = Children();
        auto it = std::find(children.begin(), children.end(), childId);
        if (it != children.end())
        {
            children.erase(it);
            return true;
        }
        return false;
    }

    bool Entity::IsDescendantOf(Entity entity) const { return entity.IsAncesterOf(*this); }
    bool Entity::IsAncesterOf(Entity entity) const
    {
        const auto& children = Children();

        if (children.empty())
            return false;

        for (UUID child : children)
        {
            if (child == entity.GetUUID())
                return true;
        }

        for (UUID child : children)
        {
            if (m_Scene->GetEntityByUUID(child).IsAncesterOf(entity))
                return true;
        }

        return false;
    }
}
