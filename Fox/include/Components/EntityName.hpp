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
    class EntityName
    {
    public:
        /**
         * @brief Constructor
         * @param name the name
         */
        EntityName(const std::string& name) : m_strName(name) {}

        /**
         * @brief Get the name
         * @return the name
         */
        std::string get() const
        {
            return m_strName;
        }

        void set(const std::string& name)
        {
            m_strName = name;
        }

    private:
        std::string m_strName;
    };

    class EntityTag
    {
    public:
        /**
         * @brief Constructor
         * @param tag the tag
         */
        EntityTag(const std::string& tag) : m_strName(tag) {}

        /**
         * @brief Get the tag
         * @return the tag
         */
        std::string get() const
        {
            return m_strName;
        }

        void set(const std::string& name)
        {
            m_strName = name;
        }

    private:
        std::string m_strName;
    };
}
#endif