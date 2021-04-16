
#include <iostream>

#include "Ecs/World.hpp"

void SystemManager::EntityDestroyed(EntityId e)
{
    for (auto& pair : m_vSystems)
    {
        auto& system = pair.second;

        system->m_vEntities.erase(e);
    }
}

void SystemManager::EntitySignatureChanged(EntityId entity, const CompSignature& entitySignature)
{
	for (auto& pair : m_vSystems)
	{
		auto& type = pair.first;
		auto& system = pair.second;

        // std::cout << "Entity: " << entitySignature << std::endl;
        // std::cout << "System: " << system->m_Signature << std::endl;
		if ((entitySignature & system->m_Signature) == system->m_Signature)
		{
			bool bIsExist = system->m_vEntities.find(entity) != system->m_vEntities.end();
			system->m_vEntities.insert(entity);

			// Si il n'existe pas, Envoyez l'event OnAdd
			if (!bIsExist) {
				system->OnAddEntity(entity);
			} else {
				system->OnSetEntity(entity);
			}
		}
		else
		{
			bool bIsExist = system->m_vEntities.find(entity) != system->m_vEntities.end();
			if (bIsExist)
			{
				system->OnRemoveEntity(entity);
			}
			system->m_vEntities.erase(entity);
		}
	}
}