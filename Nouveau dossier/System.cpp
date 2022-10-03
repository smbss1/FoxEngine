/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** System.cpp
*/

#include <iostream>
#include <algorithm>

#include "Ecs/World.hpp"

namespace fox
{
    void SystemManager::EntityDestroyed(EntityId e)
    {
        for (auto &pair : m_vSystems) {
            auto &system = pair.second;

            auto it = std::find(system->m_vEntities.begin(), system->m_vEntities.end(), e);
            if (it != system->m_vEntities.end()) {
                system->m_vEntities.erase(it);
            }
        }
    }

    void SystemManager::Clear()
    {
        for (auto &pair : m_vSystems) {
            auto &system = pair.second;
            system->m_vEntities.clear();
        }
    }

    void SystemManager::EntitySignatureChanged(EntityId entity, const CompSignature &entitySignature)
    {
        for (auto &pair : m_vSystems) {
            auto &type = pair.first;
            auto &system = pair.second;
            auto it = std::find(system->m_vEntities.begin(), system->m_vEntities.end(), entity);
            bool bIsExist = it != system->m_vEntities.end();

            if ((entitySignature & system->m_Signature) == system->m_Signature)
            {
                // Si il n'existe pas, Envoyez l'event OnAdd
                if (!bIsExist) {
                    system->m_vEntities.push_back(entity);
                    system->OnAddEntity(entity);
                } else {
                    system->OnSetEntity(entity);
                }
            } else {
                if (bIsExist) {
                    system->OnRemoveEntity(entity);
                    system->m_vEntities.erase(it);
                }
            }
        }
    }
}