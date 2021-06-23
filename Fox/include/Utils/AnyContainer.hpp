//
// Created by samuel on 22/06/2021.
//

#ifndef ECS_ANYCONTAINER_HPP
#define ECS_ANYCONTAINER_HPP

#include "Any.hpp"
#include "Option.hpp"

namespace fox
{
    class AnyContainer
    {
    public:
        template<typename T, typename... Args>
        T &set(Args &&... args)
        {
            std::type_index id = typeid(T);

            return set_any(id, Any::from<T>(args...)).template as<T>();
        }

        template<typename T>
        fox::Option<T &> get()
        {
            std::type_index id = typeid(T);
            fox::Option<Any &> res = get_any(id);

            if (res)
                return fox::just(res.value().as<T>());
            return {};
        }

        template<typename T>
        fox::Option<const T &> get() const
        {
            std::type_index id = typeid(T);
            fox::Option<const Any &> res = get_any(id);

            if (res)
                return fox::just(res.value().as<T>());
            return {};
        }

        template<typename T>
        void remove()
        {
            remove_any(typeid(T)).value().as<T>();
        }

    private:
        Any &set_any(std::type_index id, Any any)
        {
            m_vResources.erase(id);
            m_vResources.insert(std::make_pair(id, std::move(any)));
            return m_vResources.at(id);
        }

        fox::Option<Any &> get_any(std::type_index id)
        {
            auto it = m_vResources.find(id);

            if (it != m_vResources.end()) {
                return fox::just(it->second);
            } else
                return {};
        }

        fox::Option<const Any &> get_any(std::type_index id) const
        {
            auto it = m_vResources.find(id);

            if (it != m_vResources.end()) {
                return fox::just(it->second);
            } else
                return {};
        }

        fox::Option<Any> remove_any(std::type_index id)
        {
            auto it = m_vResources.find(id);

            if (it != m_vResources.end()) {
                auto any = std::move(it->second);
                m_vResources.erase(id);
                return fox::just(std::move(any));
            } else
                return {};
        }

        std::unordered_map<std::type_index, Any> m_vResources;
    };
}

#endif //ECS_ANYCONTAINER_HPP
