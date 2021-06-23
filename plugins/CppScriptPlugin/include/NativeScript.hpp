/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** NativeScript.hpp
*/

#ifndef ECS_NATIVESCRIPT_HPP
#define ECS_NATIVESCRIPT_HPP

#include "ScriptableBehaviour.hpp"

struct NativeScript
{
    using BehaviourContainer
    = std::unordered_map<std::type_index, std::unique_ptr<ScriptableBehaviour>>;

    BehaviourContainer m_vScripts;
    ScriptableBehaviour* m_pLastRegistered = nullptr;
    fox::World* m_pWorld = nullptr;

    /**
     * @brief Constructor
     */
    template <typename... Sb>
    NativeScript(Sb&&... scriptable)
    {
        (add(std::forward<Sb>(scriptable)), ...);
    }

    ~NativeScript() = default;

    template <typename S>
    void add(S scriptable)
    {
        m_vScripts.insert({
            typeid(S),
            std::make_unique<S>(std::move(scriptable)),
       });
        m_pLastRegistered = m_vScripts[typeid(S)].get();
        // This line will not be called in the constructor because it set to null
        // but later when the user add a new script it will works fine
        if (m_pWorld)
            m_pWorld->run_phase(fox::ecs::OnAddScript);
    }

    template <typename S>
    void remove()
    {
        m_vScripts[typeid(S)]->on_destroy();
        m_vScripts.erase(typeid(S));
    }

    template <typename S>
    S* get()
    {
        auto iter = m_vScripts.find(typeid(S));

        if (iter != m_vScripts.end())
            return reinterpret_cast<S*>(iter->second.get());
        return nullptr;
    }

    template <typename S>
    const S* get() const
    {
        auto iter = m_vScripts.find(typeid(S));

        if (iter != m_vScripts.end())
            return reinterpret_cast<const S*>(iter->second.get());
        return nullptr;
    }

    /**
     * @brief Call the on_update function of all script
     */
    void on_update()
    {
        for (auto& script : m_vScripts)
        {
            script.second->on_update();
        }
    }

    /**
     * @brief Call once the on_create function of the last created script
     */
    void on_create(EntityId id, fox::Application& app, fox::Scene& scene)
    {
        if (m_pLastRegistered) {
            m_pLastRegistered->set_entity(id);
            m_pLastRegistered->set_world(*m_pWorld);
            m_pLastRegistered->set_scene(scene);
            m_pLastRegistered->set_app(app);
            m_pLastRegistered->on_create();
        }
        m_pLastRegistered = nullptr;
    }

    /**
     * @brief Call once the on_create function of the array of script
     */
    void on_create_all(EntityId id, fox::Application& app, fox::Scene& scene)
    {
        for (auto& script : m_vScripts)
        {
            script.second->set_entity(id);
            script.second->set_world(*m_pWorld);
            script.second->set_scene(scene);
            script.second->set_app(app);
            script.second->on_create();
        }
    }
};

#endif //ECS_NATIVESCRIPT_HPP
