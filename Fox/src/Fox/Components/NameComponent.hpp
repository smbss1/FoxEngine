/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** NameComponent.hpp
*/

#ifndef FOX_ENTITY_NAME_HPP_
#define FOX_ENTITY_NAME_HPP_

#include <string>

namespace fox
{
    struct NameComponent
    {
        std::string name;
        /**
         * @brief Default Constructor
         * @param tag the tag
         */
        NameComponent() = default;

        /**
         * @brief Default Copy Constructor
         */
        NameComponent(const NameComponent&) = default;

        /**
         * @brief Constructor
         * @param tag the tag
         */
        explicit NameComponent(const std::string& strName) : name(strName) {}
    };
}
#endif
