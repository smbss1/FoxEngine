//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_COMPONENT_HPP
#define FOXENGINE_COMPONENT_HPP

#include <rttr/type>
#include "common.hpp"
#include "Reflection.hpp"
//#include <Ecs/World.hpp>

template <typename C>
using chandle = std::weak_ptr<C>;

struct Component : public std::enable_shared_from_this<Component>
{
    Component() = default;
    Component(const Component&) { }
    virtual ~Component() = default;

    REFLECTABLEV(Component);
    virtual CompId Id() const = 0;

    /// Owning entity
//    fox::Entity m_oEntity;
};

template <typename T>
class ComponentImpl : public Component
{
//    REFLECTABLEV(Component)

private:
    CompId Id() const override
    {
        return static_id();
    }

public:
    ComponentImpl() = default;
    ComponentImpl(const ComponentImpl& rhs) = default;
    ComponentImpl(ComponentImpl& rhs) = delete;
    ComponentImpl& operator=(ComponentImpl& rhs) = delete;

    static CompId static_id()
    {
        return typeid(T).hash_code();
    }

    chandle<T> handle()
    {
        return std::static_pointer_cast<T>(shared_from_this());
    }
};

#endif //FOXENGINE_COMPONENT_HPP
