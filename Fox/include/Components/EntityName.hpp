/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** EntityName.hpp
*/

#ifndef FOX_ENTITY_NAME_HPP_
#define FOX_ENTITY_NAME_HPP_

#include <string>

namespace fox
{
    struct EntityName : public ComponentImpl<EntityName>
    {
        REFLECTABLEV(EntityName, Component);

        std::string name;

    public:
        /**
         * @brief Default Constructor
         * @param tag the tag
         */
        EntityName() = default;

        /**
         * @brief Default Copy Constructor
         */
        EntityName(const EntityName&) = default;

        /**
         * @brief Constructor
         * @param tag the tag
         */
        explicit EntityName(const std::string& strName) : name(strName) {}
    };

    struct EntityTag : public ComponentImpl<EntityName>
    {
        REFLECTABLEV(EntityTag, Component);

        std::string tag;

    public:
        /**
         * @brief Default Constructor
         * @param tag the tag
         */
        EntityTag() = default;

        /**
         * @brief Default Copy Constructor
         */
        EntityTag(const EntityTag&) = default;

        /**
         * @brief Constructor
         * @param tag the tag
         */
        explicit EntityTag(const std::string& strTag) : tag(strTag) {}
    };
}
#endif