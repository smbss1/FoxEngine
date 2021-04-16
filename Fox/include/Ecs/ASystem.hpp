
#ifndef FOX_ASYSTEM_HPP_
#define FOX_ASYSTEM_HPP_

#include <set>
#include "common.hpp"
#include "Pipeline.hpp"

class World;

class ASystem
{
public:
	explicit ASystem(World& oWorld, std::size_t num_components)
		: m_oWorld(oWorld), m_Signature(num_components), m_pPipeline(nullptr)
	{
	}
	~ASystem() = default;
	virtual void OnAddEntity(EntityId oEntity) = 0;
	virtual void OnSetEntity(EntityId oEntity) = 0;
	virtual void OnRemoveEntity(EntityId oEntity) = 0;
	virtual void run() = 0;
	virtual void run(EntityId e) = 0;

	Pipeline* get_pipeline() const
	{
		return m_pPipeline.get();
	}

	void set_pipeline(Pipeline::SPtr pPipe)
	{
		m_pPipeline = pPipe;
	}

public:
	World& m_oWorld;
	std::set<EntityId> m_vEntities;
    CompSignature m_Signature;
	std::uint32_t m_uEventID;

protected:
	Pipeline::SPtr m_pPipeline;
};

#endif