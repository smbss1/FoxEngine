
#ifndef FOX_SCRIPTABLE_ENTITY
#define FOX_SCRIPTABLE_ENTITY

#include "Option.hpp"
#include "World.hpp"

class ScriptableEntity
{
public:
    virtual ~ScriptableEntity() {}

public:
    virtual void OnCreate() {}
    virtual void OnUpdate() {}
    virtual void OnDestroy() {}

    // Function Wrapper to the entity object
    template <typename T, typename... Args>
    T &AddComponent(Args &&...args)
    {
        m_pEntity.add<T>(std::forward<Args>()...);
        return m_pEntity.get<T>().value();
    }

    template <typename T>
    fox::Option<T &> GetComponent()
    {
        return m_pEntity.get<T>();
    }

    template <typename T>
    void SetComponent(T &&value)
    {
        return m_pEntity.set<T>(std::forward<T>(value));
    }

public:
    Entity m_pEntity;
};

#endif