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
    using ScriptPtr = std::unique_ptr<ScriptableBehaviour>;
    using Container = std::unordered_map<std::size_t, ScriptPtr>;

    Container m_vScripts;
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
        add(typeid(S).hash_code(), std::make_unique<S>(std::move(scriptable)));
    }

    template <typename S>
    void remove()
    {
        remove(typeid(S).hash_code());
    }

    template <typename S>
    S* get()
    {
        auto ptr = get(typeid(S).hash_code());
        return ptr ? ptr : nullptr;
    }

    template <typename S>
    const S* get() const
    {
        auto ptr = get(typeid(S).hash_code());
        return ptr ? ptr : nullptr;
    }

    void add(std::size_t tag, ScriptPtr scriptable)
    {
        m_vScripts.insert({ tag, std::move(scriptable) });
        m_pLastRegistered = m_vScripts[tag].get();
        // This line will not be called in the constructor because it set to null
        // but later when the user add a new script it will works fine
        if (m_pWorld)
            m_pWorld->run_phase(fox::ecs::OnAddScript);
    }

    void remove(std::size_t tag)
    {
        auto it = m_vScripts.find(tag);
        if (it != m_vScripts.end()) {
            it->second->on_destroy();
            m_vScripts.erase(it);
        }
    }

    ScriptableBehaviour* get(std::size_t tag)
    {
        auto iter = m_vScripts.find(tag);

        if (iter != m_vScripts.end())
            return iter->second.get();
        return nullptr;
    }

    const ScriptableBehaviour* get(std::size_t tag) const
    {
        auto iter = m_vScripts.find(tag);

        if (iter != m_vScripts.end())
            return iter->second.get();
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
    void on_create(EntityId id, fox::Application& app)
    {
        if (m_pLastRegistered) {
            m_pLastRegistered->set_entity(id);
            m_pLastRegistered->set_world(*m_pWorld);
            m_pLastRegistered->set_app(app);
            m_pLastRegistered->on_create();
        }
        m_pLastRegistered = nullptr;
    }

    /**
     * @brief Call once the on_create function of the array of script
     */
    void on_create_all(EntityId id, fox::Application& app)
    {
        for (auto& script : m_vScripts)
        {
            script.second->set_entity(id);
            script.second->set_world(*m_pWorld);
            script.second->set_app(app);
            script.second->on_create();
        }
    }
};

using ScriptCreator = std::unique_ptr<ScriptableBehaviour> (*)();
using StringHash = std::hash<std::string>;

template <typename script_class>
std::unique_ptr<ScriptableBehaviour> CreateScript()
{
    return std::make_unique<script_class>();
}

bool RegisterScript(size_t tag, ScriptCreator func);
bool RegisterScriptName(const char* name);

#define REGISTER_SCRIPT(type) \
bool reg {RegisterScript(StringHash()(#type), CreateScript<type>)}; \
bool name{ RegisterScriptName(#type)}; \

#endif //ECS_NATIVESCRIPT_HPP
