/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Pipeline.hpp
*/

#ifndef FOX_PIPELINE_HPP_
#define FOX_PIPELINE_HPP_

#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include "Utils/bitsets.hpp"

namespace fox
{
    class ASystem;

    class Pipeline
    {
    public:
        using SPtr = std::shared_ptr<Pipeline>;
        using UPtr = std::unique_ptr<Pipeline>;

        /**
         * @brief Constructor of the class
         */
        Pipeline() = default;

        /**
         * @brief Destructor
         */
        ~Pipeline() = default;

        /**
         * @brief Add a new phase (used for systems)
         * @param id the id of the event
         */
        void add_phase(std::uint32_t id);

        /**
         * @brief Add a system to a phase
         * @param id the pahse id
         * @param pSys the system to add in the phase
         */
        void add_system(std::uint32_t id, ASystem *pSys);

        /**
         * @brief Remove a system to a phase
         * @param id the pahse id
         * @param pSys the system to remove in the phase
         */
        void remove(std::uint32_t id, ASystem *pSys);

        /**
         * @brief Run a phase
         * @param id the id of the event
         */
        void run_phase(std::uint32_t id);

        /**
         * @brief Check if a phase exist or not
         * @param id the id of the event
         * @return true if the phase id exist otherwise false
         */
        bool has_phase(std::uint32_t id);

    private:
        std::unordered_map<std::uint32_t, std::vector<ASystem *>> m_vPhases;
    };
}
#endif