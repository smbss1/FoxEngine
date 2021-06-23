/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Pipeline.cpp
*/

#include <iostream>
#include <algorithm>
#include "Ecs/Pipeline.hpp"
#include "Ecs/ASystem.hpp"
namespace fox
{
    Pipeline::Pipeline()
    {
    }

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
        }
    }

    void Pipeline::run_phase(std::uint32_t id)
    {
        try {
            auto it = m_vPhases.find(id);
            if (it != m_vPhases.end())
                for (auto &sys : it->second) {
                    sys->run();
                }
        } catch (const std::out_of_range &e) {
        }
    }

    bool Pipeline::has_phase(std::uint32_t id)
    {
        return m_vPhases.find(id) != m_vPhases.end();
    }
}
