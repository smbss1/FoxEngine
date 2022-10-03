/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** World.hpp
*/

#ifndef FOX_WORLD_HPP_
#define FOX_WORLD_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <Utils/AnyContainer.hpp>
#include <Core/Logger/Logger.hpp>
#include "common.hpp"
#include "Utils/Option.hpp"
#include "Utils/Any.hpp"
#include "ASystem.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "Pipeline.hpp"
#include "Components/IDComponent.hpp"

namespace fox
{
    class Entity;

    class World;

    template<class... Cs>
    class System : public ASystem
    {
    public:
        using SystemFn = std::function<void(Entity& oEntity, Cs&...)>;
        using OrderFn = std::function<bool(Cs&..., Cs&...)>;
        std::string m_strName;                                            // The name of the system, used to generate unique Id for the different Event Phases like OnAdd or OnRemove
        SystemFn m_oFunc;
        OrderFn m_oOrderByFunc;

        /**
         * @brief Construct a new System object
         *
         * @param oWorld a reference to the world
         * @param num_components the number of registered components in the world
         */
        System(World &oWorld, std::size_t num_components);

        /**
         * @brief Store the function for this System that will be called when
         * it receive an event
         *
         * @param updateFunc the function that will called
         */
        void each(SystemFn updateFunc)
        {
            m_oFunc = std::move(updateFunc);
        }

        System<Cs...>& order_by(OrderFn orderFn)
        {
            m_oOrderByFunc = std::move(orderFn);
            return *this;
        }

        /**
         * @brief Set the event to listen
         *
         * @param id the id of the event
         * @return System<Cs...>& a reference to it self
         */
        System<Cs...> &kind(std::uint32_t id)
        {
            bool is_trigger = id == fox::ecs::OnAdd ||
                              id == fox::ecs::OnRemove ||
                              id == fox::ecs::OnSet;
            if (m_pPipeline) {
                if (!is_trigger) {
                    m_pPipeline->remove(m_uEventID, this);
                    m_pPipeline->add_system(id, this);
                } else
                    m_pPipeline->remove(m_uEventID, this);
            }
            m_uEventID = id;
            return *this;
        }

        System<Cs...>& force_update(bool value)
        {
            update_on_entity_disable = value;
            return *this;
        }

        /**
         * @brief Run the system
         */
        void run();

        /**
         * @brief Run the system
         * @param e the entity id
         */
        void run(EntityId e);

        /**
         * @brief This function will be called in the world, when trigger the OnAdd event
         *
         * @param oEntity the entity id
         */
        void OnAddEntity(EntityId oEntity);

        /**
         * @brief This function will be called in the world, when trigger the OnSet event
         *
         * @param oEntity the entity id
         */
        void OnSetEntity(EntityId oEntity);

        /**
         * @brief This function will be called in the world, when trigger the OnRemove event
         *
         * @param oEntity the entity id
         */
        void OnRemoveEntity(EntityId oEntity);
    };

    class SystemManager
    {
    public:
        using Ptr = std::unique_ptr<SystemManager>;

        template<class Cs>
        void process_one_type(World *wrld, CompSignature &sig);

        /**
         * @brief Create a system object
         *
         * @tparam Cs a list of Components that the system needs
         * @param pWorld a pointer to the world
         * @param strName The name of the system
         * @param num_components the number of registered components in the world
         * @return System<Cs...>& a reference to the created system
         */
        template<class... Cs>
        System<Cs...> &create_system(World *pWorld, std::string strName, std::size_t num_components)
        {
            if (strName.empty())
                strName = "System_" + std::to_string(m_vSystems.size());

            // assert(m_vSystems.find(name.c_str()) == m_vSystems.end() && "Registering system more than once.");

            std::unique_ptr<System<Cs...>> systemPtr = std::make_unique<System<Cs...>>(*pWorld, num_components);
            auto *system = systemPtr.get();
            int _[] = {0, (process_one_type<Cs>(pWorld, system->m_Signature), 0)...};
            system->m_strName = strName;
            m_vSystems[strName] = std::move(systemPtr);
            return *system;
        }

        /**
         * @brief This function allows to delete a entity in the system list
         *
         * @param entity the deleted entity
         */
        void EntityDestroyed(EntityId entity);

        /**
         * @brief Clear the entity list of all systems
         */
        void Clear();

        /**
         * @brief This function remove/add the entity, given in parameter, in the system list and it trigger a OnAdd/OnRemove event
         *
         * @param entity the changed entity
         * @param entitySignature the changed signature of the entity
         */
        void EntitySignatureChanged(EntityId entity, const CompSignature &entitySignature);

    public:
        std::unordered_map<std::string, std::unique_ptr<ASystem>> m_vSystems;
    };

    class World
    {
    public:
        /**
         * @brief Construct a new World object
         */
        World();

        /**
         * @brief Destroy the World object
         */
        ~World() = default;

        /**
         * @brief Create a new entity in the world
         *
         * @return Entity the entity
         */
        Entity new_entity();
        Entity new_entity(const std::string &name);
        Entity new_entity(const std::string &name, const std::string &tag_name);
        Entity new_entity(const std::string &name, bool enable);
        Entity new_entity(const std::string &name, const std::string &tag_name, bool enable);

        /**
         * @brief Get an entity match with the given name
         * @param name the name of the entity
         * @param[out] e the entity found
         * @return true if the entity is found otherwise false
         */
        bool get_entity(const std::string &name, Entity &e);

        /**
         * @brief Get an entity match with the given tag
         * @param tag_name the tag of the entity
         * @param[out] e the entity found
         * @return true if the entity is found otherwise false
         */
        bool get_entity_with_tag(const std::string &tag_name, Entity &e);

        /**
         * @brief Get all entities match with the given tag
         * @param tag_name the tag of the entity
         * @param[out] e the list of entity found
         * @return true if the list is not empty otherwise false
         */
        bool get_entities_with_tag(const std::string &tag_name, std::vector<fox::Entity>& vE);

        /**
         * @brief Get Entity list
         * @return the array of entity
         */
        std::vector<EntityId>& get_entities() const;

        /**
         * @brief Add a component to a signature
         * @tparam Cs the type of the component
         * @param sig the signature
         */
        template<typename Cs>
        void set_component_signature(CompSignature &sig)
        {
            sig.set(get_type_idx<Cs>(), true);
        }

        /**
         * @brief Get entities match with the set of specified components
         * @tparam Cs the component type list
         * @return the list of founded entities
         */
        template<typename... Cs>
        std::vector<fox::Entity> get_entities_with();

        void clear_entities();

        /**
         * @brief Remove an entity in the world
         * @param e the entity to remove
         */
        void delete_entity(EntityId e);
//
//        /**
//         * @brief Called at the end of the frame to remove all entity to be removed
//         */
//        void deleted_entities();

        void each(const std::function<void(EntityId)>& fn);
        void each(const std::function<bool(EntityId)>& fn);

        template<typename T>
        void RegisterComponent()
        {
            m_pCompManager->template RegisterComponent<T>();
        }

        /**
         * @brief Add a component to the entity
         *
         * @tparam T the type of the component
         * @tparam Args the list of arguments to pass to the constuctor of the component
         * @param e the entity id
         * @param args the arguments to pass to the constuctor of the component
         * @return T& a reference the component
         */
        template<typename T, typename... Args>
        T &add_component(EntityId e, Args &&... args)
        {
            T &comp = m_pCompManager->AddComponent<T>(e, std::forward<Args>(args)...);

            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType<T>(), true);
            m_pSysManager->EntitySignatureChanged(e, signature);
            return comp;
        }

        /**
         * @brief Add component to the entity index
         * @tparam T the type of the Component
         * @param e the entity id
         * @return T& reference to the Component
         */
        template<typename T>
        T &add_component(EntityId e)
        {
            T &comp = m_pCompManager->AddComponent<T>(e);
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType<T>(), true);
            m_pSysManager->EntitySignatureChanged(e, signature);
            return comp;
        }

        /**
         * @brief Get the component object
         *
         * @tparam T the type of the component
         * @param e the entity id
         * @return fox::Option<T&> an optionnal value
         */
        template<typename T>
        fox::ref<T> get_component(EntityId e)
        {
            return m_pCompManager->GetComponent<T>(e);
        }

        std::vector<ref<Component>> GetAllComponents(EntityId entityId)
        {
            return m_pCompManager->GetAllComponents(entityId, m_pEntityManager->GetSignature(entityId));
        }

        template<typename T>
        void emplace_or_replace(EntityId e, const T& component)
        {
            if (get_component<T>(e) != nullptr)
            {
                auto opt = get_component<T>(e);
                if (opt)
                    *opt = component;
                auto &signature = m_pEntityManager->GetSignature(e);
                signature.set(m_pCompManager->GetComponentType<T>(), true);
                m_pSysManager->EntitySignatureChanged(e, signature);
            }
            else
            {
                add_component<T>(e);
//                set_component<T>(e, component);
            }
        }

        /**
         * @brief Set the value of a component
         * @tparam T the type of the component
         * @param e the entity
         * @param value the new component value
         */
        template<typename T>
        void set_component(EntityId e, T&& value)
        {
            auto opt = get_component<T>(e);
            if (opt)
                *opt = value;
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType<T>(), true);
            m_pSysManager->EntitySignatureChanged(e, signature);
        }

        void set_component(EntityId e, ref<Component> component)
        {
            m_pCompManager->SetComponent(e, component);
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType(component), true);
            m_pSysManager->EntitySignatureChanged(e, signature);
        }

        /**
         * @brief remove a component to an entity
         *
         * @tparam T the type of the component to be remove
         * @param e the entity id
         */
        template<typename T>
        void remove_component(EntityId e)
        {
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType<T>(), false);
            m_pSysManager->EntitySignatureChanged(e, signature);
            m_pCompManager->RemoveComponent<T>(e);
        }

        void remove_component(EntityId e, ref<Component> pComponent)
        {
            auto &signature = m_pEntityManager->GetSignature(e);
            signature.set(m_pCompManager->GetComponentType(pComponent), false);
            m_pSysManager->EntitySignatureChanged(e, signature);
            m_pCompManager->RemoveComponent(e, pComponent);
        }


        /**
         * @brief Get the index of a registered component
         * @tparam T the type of the component
         * @return the index
         */
        template<typename T>
        CompId get_type_idx()
        {
            return m_pCompManager->GetComponentType<T>();
        }

        /**
         * @brief Enable an entity
         * @param e the entity id
         * @param v the enable value
         */
        void enable_entity(EntityId e, bool v)
        {
            m_pEntityManager->enable_entity(e, v);
        }

        /**
         * @brief Check if an entity is enbale or not
         * @param eId the entity id
         * @return true if entity is enable otherwise false
         */
        bool is_entity_enable(EntityId eId)
        {
            return m_pEntityManager->is_entity_enable(eId);
        }

        /**
         * @brief Set a new resource
         * @tparam T the type of the resource
         * @tparam Args the arguments type for constructor of the resource
         * @param args the arguments for constructor of the resource
         * @return the new resource
         */
        template<typename T, typename... Args>
        T &set(Args &&... args)
        {
            return m_vAny.template set<T>(args...);
        }

        /**
         * @brief Get the resource
         * @tparam T the type of the resource
         * @return the resource
         */
        template<typename T>
        fox::Option<T &> get()
        {
            return m_vAny.template get<T>();
        }

        /**
         * @brief Get the resource
         * @tparam T the type of the resource
         * @return the resource
         */
        template<typename T>
        fox::Option<const T &> get() const
        {
            return m_vAny.template get<T>();
        }

        /**
         * @brief Remove a resource
         * @tparam T the type of the resource
         * @return the removed resource
         */
        template<typename T>
        fox::Option<T> remove()
        {
            return m_vAny.template remove<T>();
        }

        /**
         * @brief Create a new system and add it to the world
         *
         * @tparam Cs a list of Components that the system needs
         * @param strName The name of the system
         * @return System<Cs...>& a reference to the created system
         */
        template<typename... Cs>
        System<Cs...> &system(std::string strName = "")
        {
            std::size_t num_comps = m_pCompManager->GetSize() + 1;
            auto &system = m_pSysManager->create_system<Cs...>(this, strName, num_comps);
            return system;
        }

        /**
         * @brief Add a new phase (used for systems)
         * @param id the id of the event
         */
        void add_phase(uint32_t id)
        {
            m_pPipeline->add_phase(id);
        }

        /**
         * @brief Run a phase
         * @param id the id of the event
         */
        void run_phase(uint32_t id)
        {
            m_pPipeline->run_phase(id);
        }

        /**
         * @brief Set the pipeline
         * @param pipe the pipeline
         */
        void set_pipeline(Pipeline::SPtr &pipe)
        {
            m_pPipeline = pipe;
        }

    private:

        AnyContainer m_vAny;
        SystemManager::Ptr m_pSysManager;
        ComponentManager::Ptr m_pCompManager;
        EntityManager::Ptr m_pEntityManager;
        Pipeline::SPtr m_pPipeline;
    };

    class Entity
    {
    public:
        /**
         * @brief Construct a new Entity object
         */
        Entity() : m_oWld(nullptr), m_iId(-1)
        {}

        /**
         * @brief Construct a new Entity object
         *
         * @param w the world
         * @param id the id of the entity
         */
        Entity(World *w, EntityId id) : m_oWld(w), m_iId(id)
        {
        }

        /**
         * @brief Construct a new Entity object
         */
        Entity(const Entity &) = default;

        bool operator==(const Entity &e) const;

        bool operator!=(const Entity &other) const;
        operator bool () const { return m_iId != -1; }
        operator EntityId () const { return m_iId; }

        Entity &operator=(const Entity &other)
        {
            m_iId = other.m_iId;
            m_oWld = other.m_oWld;
            return *this;
        }

        /**
         * @brief Remove the entity to the world
         */
        void destroy();

        /**
         * @brief Check if an entity is enbale or not
         * @return true if the entity is enable otherwise false
         */
        bool is_enable()
        {
            return m_oWld->is_entity_enable(m_iId);
        }

        /**
         * @brief Enable the entity
         * @param v the enable value
         */
        void set_enable(bool v)
        {
            m_oWld->enable_entity(m_iId, v);
        }

        /**
         * @brief Add a component to the entity
         *
         * @tparam T the type of the component
         * @tparam Args the list of arguments to pass to the constuctor of the component
         * @param args the arguments to pass to the constuctor of the component
         * @return Enity& the class itself
         */
        template<typename T, typename... Args>
        T &add(Args &&... args)
        {
            return m_oWld->add_component<T>(m_iId, std::forward<Args>(args)...);
        }

        /**
         * @brief Add component to the entity index
         * @tparam T the type of the Component
         * @return Enity& the class itself
         */
        template<typename T>
        T &add()
        {
            return m_oWld->add_component<T>(m_iId);
        }

        /**
         * @brief Get the component object
         *
         * @tparam T the type of the component
         * @return fox::Option<T&> an optionnal value
         */
        template<typename T>
        fox::ref<T> get()
        {
            if (m_oWld)
                return m_oWld->get_component<T>(m_iId);
            return {};
        }

        uint64_t get_uuid()
        {
            return get<IDComponent>()->ID;
        }

        template<typename T>
        bool has()
        {
            return get<T>() != nullptr;
        }

        std::vector<ref<Component>> GetAll()
        {
            if (m_oWld)
                return std::move(m_oWld->GetAllComponents(m_iId));
            return {};
        }


        /**
         * @brief Set the value of a component
         * @tparam T the type of the component
         * @param value the new component value
         */
        template<typename T>
        Entity &set(T &&value)
        {
            if (m_oWld)
                m_oWld->set_component<T>(m_iId, std::forward<T>(value));
            return *this;
        }

        Entity &set(ref<Component> component)
        {
            if (m_oWld)
                m_oWld->set_component(m_iId, component);
            return *this;
        }

        /**
         * @brief remove a component to an entity
         *
         * @tparam T the type of the component to be remove
         */
        template<typename T>
        void remove()
        {
            if (m_oWld)
                m_oWld->remove_component<T>(m_iId);
        }

        void remove(ref<Component> pComponent)
        {
            if (m_oWld)
                m_oWld->remove_component(m_iId, pComponent);
        }

        /**
         * @brief Get the id of the entity
         * @return the id
         */
        EntityId get_id() const
        {
            return m_iId;
        }

        /**
         * @brief Get World where the entity is spawned
         * @return the Ecs World
         */
        [[nodiscard]] World *get_world() const
        {
            return m_oWld;
        }

    private:
        World *m_oWld = nullptr;
        EntityId m_iId;
    };

    template<typename... Cs>
    System<Cs...>::System(World &oWorld, std::size_t num_components)
            : ASystem(oWorld, num_components)
    {
        m_pPipeline = oWorld.get<Pipeline::SPtr>().value();
        kind(fox::ecs::OnUpdate);
    }

    template<typename... Cs>
    void System<Cs...>::OnAddEntity(EntityId oEntity)
    {
        if (m_oOrderByFunc)
        {
            std::vector<Entity> vec;
            for (auto e : m_vEntities)
                vec.push_back({&m_oWorld, e});
            std::sort(vec.begin(), vec.end(), [&](Entity& e1, Entity& e2)
            {
                return m_oOrderByFunc(*e1.get<Cs>()..., *e2.get<Cs>()...);
            });
            m_vEntities.clear();
            for (auto e : vec)
                m_vEntities.push_back(e.get_id());
        }
        if (m_uEventID == fox::ecs::OnAdd) {
            run(oEntity);
        }
    }

    template<typename... Cs>
    void System<Cs...>::OnSetEntity(EntityId oEntity)
    {
        if (m_uEventID == fox::ecs::OnSet) {
            run(oEntity);
        }
    }

    template<typename... Cs>
    void System<Cs...>::OnRemoveEntity(EntityId oEntity)
    {
        if (m_uEventID == fox::ecs::OnRemove) {
            run(oEntity);
        }
    }

    template<typename... Cs>
    void System<Cs...>::run()
    {
        for (auto e : m_vEntities)
        {
            run(e);
        }
    }

    template<typename... Cs>
    void System<Cs...>::run(EntityId e)
    {
        if (!m_oWorld.is_entity_enable(e) && !update_on_entity_disable)
            return;
        Entity ent(&m_oWorld, e);
        try {
            m_oFunc(ent, *m_oWorld.template get_component<Cs>(e)...);
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

    template<class Cs>
    void SystemManager::process_one_type(World *wrld, CompSignature &sig)
    {
        sig.set(wrld->get_type_idx<Cs>(), true);
    }

    template<typename... Cs>
    std::vector<fox::Entity> World::get_entities_with()
    {
        CompSignature signature(m_pCompManager->GetSize() + 1);
        int _[] = {0, (set_component_signature<Cs>(signature), 0)...};
        std::vector<fox::Entity> ent;
        m_pEntityManager->ForEach([&](EntityId eId) -> bool
          {
              auto sig = m_pEntityManager->GetSignature(eId);
              if ((signature & sig) == signature)
                  ent.push_back(fox::Entity{this, eId});
              return false;
          });
        return ent;
    }
}
#endif
