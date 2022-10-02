/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Pipeline.cpp
*/

#include <iostream>
#include <algorithm>
#include <Core/Logger/Logger.hpp>
#include "Ecs/Pipeline.hpp"
#include "Ecs/ASystem.hpp"
namespace fox
{
    void Pipeline::add_phase(std::uint32_t id)
    {
        m_vPhases.emplace(id, std::vector<ASystem *>());
    }

    void Pipeline::add_system(std::uint32_t id, ASystem *pSys)
    {
        try {
            auto &phase = m_vPhases.at(id);
            phase.push_back(pSys);
        } catch (const std::out_of_range &e) {
            fox::error("Cannot add system into the event '%' because it's not exist in this Pipeline.\n Use Pipeline::add_phase before adding system", id);
        }
    }

    void Pipeline::remove(std::uint32_t id, ASystem *pSys)
    {
        try {
            auto &phase = m_vPhases.at(id);
            phase.erase(std::remove_if(phase.begin(), phase.end(),
               [pSys](const ASystem *Sys)
               {
                   return pSys == Sys;
               }), phase.end());
        } catch (const std::out_of_range &e) {
            // fox::error("Cannot remove system into the event '%' because it's not exist in this Pipeline.\n Use Pipeline::add_phase before removing system", id);
        }
    }

    void Pipeline::run_phase(std::uint32_t id)
    {
        try {
            auto& phase = m_vPhases.at(id);
            for (auto &sys : phase) {
                sys->run();
            }
        } catch (const std::out_of_range &e) {
            fox::error("Cannot run the event '%' because it's not exist in this Pipeline.\n Use Pipeline::add_phase to add the event", id);
        }
    }

    bool Pipeline::has_phase(std::uint32_t id)
    {
        return m_vPhases.find(id) != m_vPhases.end();
    }
}
