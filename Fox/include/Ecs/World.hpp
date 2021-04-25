
#ifndef FOX_WORLD_HPP_
#define FOX_WORLD_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>
#include "common.hpp"
#include "Option.hpp"
#include "Any.hpp"
#include "ASystem.hpp"
#include "NameUtils.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "Pipeline.hpp"

struct Archetype;
class Entity;
class World;

template<class... Cs>
class System : public ASystem
{
public:
	using SystemFn = std::function<void (Entity oEntity, Cs&...)>;
	std::string m_strName;											// The name of the system, used to generate unique Id for the different Event Phases like OnAdd or OnRemove
    SystemFn m_oFunc;

	/**
	 * @brief Construct a new System object
	 * 
	 * @param oWorld a reference to the world
	 * @param num_components the number of registered components in the world
	 */
	System(World& oWorld, std::size_t num_components);

	// @brief This function store a function for this System that will be called on the OnUpdate Phase
	/**
	 * @brief Store the function for this System that will be called when
	 * it receive an event
	 * 
	 * @param updateFunc the function that will called
	 */
	void each(SystemFn updateFunc)
	{
        m_oFunc = updateFunc;
	}

	// @brief This function allows to add a listener to an event
	/**
	 * @brief Set the event to listen
	 * 
	 * @param id the id of the event
	 * @return System<Cs...>& a reference to it self
	 */
	System<Cs...>& kind(std::uint32_t id)
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

	void run();
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

private:
	// void ForEach(Fox::Ecs::Event& event);
	int m_iIndex = -1;

};

class SystemManager
{
public:
	using Ptr = std::unique_ptr<SystemManager>;

    template <class Cs>
	void process_one_type(World* wrld, CompSignature& sig);

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
	System<Cs...>& create_system(World* pWorld, std::string strName, std::size_t num_components)
	{
		if (strName.empty())
			strName = "System_" + std::to_string(m_vSystems.size());
		
		// assert(m_vSystems.find(name.c_str()) == m_vSystems.end() && "Registering system more than once.");

		std::unique_ptr<System<Cs...>> systemPtr = std::make_unique<System<Cs...>>(*pWorld, num_components);
        auto* system = systemPtr.get();
		int _[] = {0, (process_one_type<Cs>(pWorld, system->m_Signature), 0)...};
		system->m_strName = strName;
		m_vSystems[strName] = std::move(systemPtr);
		return *system;
	}

	// @brief This function allows to add a Signature to the System who match with the name given in parameter
	// void SetSignature(Signature signature, std::string& name)
	// {
	// 	assert(m_vSystems.find(name.c_str()) != m_vSystems.end() && "System used before registered.");

	// 	m_vSignatures.insert({name.c_str(), signature});
	// }

	// // @brief This function allows to get the Signature who match with the name given in parameter
	// void GetSignature(std::string& name)
	// {
	// 	if (m_vSystems.find(name) != m_vSystems.end())
	// 		m_vSignatures[name];
	// 	else
	// 		std::cerr << "The System '" << name << "' have no signature.";
	// }

	/**
	 * @brief This function allows to delete a entity in the system list
	 * 
	 * @param entity the deleted entity
	 */
	void EntityDestroyed(EntityId entity);

	/**
	 * @brief This function remove/add the entity, given in parameter, in the system list and it trigger a OnAdd/OnRemove event
	 * 
	 * @param entity the changed entity
	 * @param entitySignature the changed signature of the entity
	 */
	void EntitySignatureChanged(EntityId entity, const CompSignature& entitySignature);

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
    ~World();

	/**
	 * @brief Create a new entity in the world
	 * 
	 * @return Entity the entity
	 */
    Entity new_entity();

	/**
	 * @brief Remove an entity in the world
	 * @param e the entity to remove
	 */
    void delete_entity(EntityId e);

	/**
	 * @brief Add a component to the entity
	 * 
	 * @tparam T the type of the component
	 * @tparam Args the list of arguments to pass to the constuctor of the component
	 * @param e the entity id
	 * @param args the arguments to pass to the constuctor of the component
	 * @return T& a reference the component
	 */
    template <typename T, typename... Args>
    T& add_component(EntityId e, Args&&... args)
    {
        T& comp = m_pCompManager->AddComponent<T>(e, std::forward<Args>(args)...);
        auto& signature = m_pEntityManager->GetSignature(e);
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
    template <typename T>
    fox::Option<T&> get_component(EntityId e)
    {
        return m_pCompManager->GetComponent<T>(e);
    }

	template <typename T>
    void set_component(EntityId e, T&& value)
    {
        auto opt = get_component<T>(e);
        if (opt)
            opt.value() = value;
        auto& signature = m_pEntityManager->GetSignature(e);
        m_pSysManager->EntitySignatureChanged(e, signature);
    }

	/**
	 * @brief remove a component to an entity
	 * 
	 * @tparam T the type of the component to be remove
	 * @param e the entity id
	 */
	template <typename T>
    void remove_component(EntityId e)
    {
        auto& signature = m_pEntityManager->GetSignature(e);
		signature.set(m_pCompManager->GetComponentType<T>(), false);
        m_pSysManager->EntitySignatureChanged(e, signature);
        m_pCompManager->RemoveComponent<T>(e);
    }

    template <typename T>
    CompId get_type_idx()
    {
        return m_pCompManager->GetComponentType<T>();
    }

    template <typename T, typename... Args>
	T& set(Args&&... args)
	{
		std::type_index id = typeid(T);

		return set_resource(id, Any::from<T>(args...)).template as<T>();
	}

	template <typename T>
	fox::Option<T&> get()
	{
		std::type_index id = typeid(T);
        fox::Option<Any&> res = get_resource(id);

        if (res)
		    return fox::just(res.value().as<T>());
		return {};
	}

	template <typename T>
	fox::Option<const T&> get() const
	{
		std::type_index id = typeid(T);
        fox::Option<const Any&> res = get_resource(id);

        if (res)
		    return fox::just(res.value().as<T>());
		return {};
	}

	template <typename T>
	fox::Option<T> remove()
	{
		return remove_resource(typeid(T)).value().as<T>();
	}

	/**
	 * @brief Create a new system and add it to the world
	 * 
	 * @tparam Cs a list of Components that the system needs
	 * @param strName The name of the system
	 * @return System<Cs...>& a reference to the created system
	 */
    template<typename... Cs>
	System<Cs...>& system(std::string strName = "")
	{
        std::size_t num_comps = m_pCompManager->GetSize() + 1;
		auto& system = m_pSysManager->create_system<Cs...>(this, strName, num_comps);
		return system;
	}

	/* Pipeline */
	void add_phase(uint32_t id)
	{
		m_pPipeline->add_phase(id);
	}

	void run_phase(uint32_t id)
	{
		m_pPipeline->run_phase(id);
	}

	void set_pipeline(Pipeline::SPtr& pipe)
	{
		m_pPipeline = pipe;
	}

private:
    Any& set_resource(std::type_index id, Any any);
	fox::Option<Any&> get_resource(std::type_index id);
	fox::Option<const Any&> get_resource(std::type_index id) const;
	fox::Option<Any> remove_resource(std::type_index id);

	std::unordered_map<std::type_index, Any> m_vResources;
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
	 * 
	 * @param w the world
	 * @param id the id of the entity
	 */
    Entity() : m_oWld(nullptr), m_iId(-1) { }
    Entity(World* w, EntityId id) : m_oWld(w), m_iId(id)
    {
    }

	/**
	 * @brief Construct a new Entity object
	 */
    Entity(const Entity&) = default;

    bool operator==(const Entity& e) const;
    bool operator!=(const Entity& other) const;
    Entity& operator=(const Entity& other)
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
	 * @brief Add a component to the entity
	 * 
	 * @tparam T the type of the component
	 * @tparam Args the list of arguments to pass to the constuctor of the component
	 * @param args the arguments to pass to the constuctor of the component
	 * @return T& a reference the component
	 */
    template <typename T, typename... Args>
    Entity& add(Args&&... args)
    {
		if (m_oWld)
        	m_oWld->add_component<T>(m_iId, std::forward<Args>(args)...);
		return *this;
    }

	/**
	 * @brief Get the component object
	 * 
	 * @tparam T the type of the component
	 * @return fox::Option<T&> an optionnal value
	 */
    template <typename T>
    fox::Option<T&> get()
    {
		if (m_oWld)
        	return m_oWld->get_component<T>(m_iId);
    	return {};
    }

    template <typename T>
    Entity& set(T&& value)
    {
		if (m_oWld)
        	m_oWld->set_component<T>(m_iId, std::forward<T>(value));
        return *this;
    }

	/**
	 * @brief remove a component to an entity
	 * 
	 * @tparam T the type of the component to be remove
	 */
    template <typename T>
    void remove()
    {
		if (m_oWld)
        	m_oWld->remove_component<T>(m_iId);
    }

    EntityId get_id() const
    {
        return m_iId;
    }

	World* get_world() const
    {
        return m_oWld;
    }

private:
    World* m_oWld = nullptr;
    EntityId m_iId;
};

template <typename... Cs>
System<Cs...>::System(World& oWorld, std::size_t num_components)
	: ASystem(oWorld, num_components)
{
	m_pPipeline = oWorld.get<Pipeline::SPtr>().value();
	kind(fox::ecs::OnUpdate);
}

template <typename... Cs>
void System<Cs...>::OnAddEntity(EntityId oEntity)
{
    if (m_uEventID == fox::ecs::OnAdd) {
        run(oEntity);
    }
}

template <typename... Cs>
void System<Cs...>::OnSetEntity(EntityId oEntity)
{
    if (m_uEventID == fox::ecs::OnSet) {
        run(oEntity);
    }
}

template <typename... Cs>
void System<Cs...>::OnRemoveEntity(EntityId oEntity)
{
    if (m_uEventID == fox::ecs::OnRemove) {
        run(oEntity);
    }
}

template <typename... Cs>
void System<Cs...>::run()
{
	for (auto e : m_vEntities) {
		run(e);
	}
}

template <typename... Cs>
void System<Cs...>::run(EntityId e)
{
	Entity ent(&m_oWorld, e);
	m_oFunc(ent, ent.get<Cs>().value()...);
}

template <class Cs>
void SystemManager::process_one_type(World* wrld, CompSignature& sig)
{
    sig.set(wrld->get_type_idx<Cs>(), true);
}

#endif