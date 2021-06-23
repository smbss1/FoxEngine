/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ASystem.hpp
*/

#ifndef FOX_ASYSTEM_HPP_
#define FOX_ASYSTEM_HPP_

#include <set>
#include "common.hpp"
#include "Pipeline.hpp"

namespace fox
{
    class World;

    class ASystem
    {
    public:
        /**
         * @brief Contructor of the class
         * @param oWorld the Ecs world
         * @param num_components the number of components register in ComponentManager
         */
        explicit ASystem(World &oWorld, std::size_t num_components)
                : m_oWorld(oWorld), m_Signature(num_components), m_pPipeline(nullptr), m_uEventID(-1)
                , update_on_entity_disable(false)
        {
        }

        /**
         * @brief Destructor
         */
        ~ASystem() = default;

        /**
         * @brief Called when a entity match the system signature and when the system is registered to ecs::OnAdd event
         * @param oEntity the entity id
         */
        virtual void OnAddEntity(EntityId oEntity) = 0;

        /**
         * @brief Called when a entity match the system signature and when the system is registered to ecs::OnSet event
         * @param oEntity the entity id
         */
        virtual void OnSetEntity(EntityId oEntity) = 0;

        /**
         * @brief Called when a entity match the system signature and when the system is registered to ecs::OnRemove event
         * @param oEntity the entity id
         */
        virtual void OnRemoveEntity(EntityId oEntity) = 0;

        /**
         * @brief Run the function of the system
         */
        virtual void run() = 0;

        /**
         * @brief Run the function of the system
         * @param e the entity id
         */
        virtual void run(EntityId e) = 0;

        /**
         * @brief Get the pipeline
         * @return the pipeline
         */
        Pipeline *get_pipeline() const
        {
            return m_pPipeline.get();
        }

        /**
         * @brief Set the pipeline
         * @param pPipe the pipeline to set
         */
        void set_pipeline(Pipeline::SPtr pPipe)
        {
            m_pPipeline = pPipe;
        }

    public:
        World &m_oWorld;
        std::vector<EntityId> m_vEntities;
        CompSignature m_Signature;
        int m_uEventID;
        bool update_on_entity_disable;

    protected:
        Pipeline::SPtr m_pPipeline;
    };
}

#endif