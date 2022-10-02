//
// Created by samuel on 05/07/2021.
//

#ifndef FOXENGINE_TYPEDMAP_HPP
#define FOXENGINE_TYPEDMAP_HPP

#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <functional>

namespace fox
{
    template<typename T>
    void deleter(void *d)
    {
        delete (T *) d;
    }

    // an map encapsulation to store any type in the same map
    // only one instance of each type can be stored inside the map
    class typed_map
    {
    private:

        // the map, each type is referenced by it's type_index and cast to void*
        std::unordered_map<std::type_index, std::pair<void *, std::function<void(void *)>>> m_vData;

    public:

        ~typed_map()
        {
            for (auto &it : m_vData)
            {
                it.second.second(it.second.first);
            }
        }

        // add a new type instance inside the map.
        // the first type parameter is the type of the new object to insert in the map
        // the second is the list of types parameters to instanciate the type
        // the methods parameters are the list of variable to pass to the type instanciation
        // return false if instanciation failed, true otherwise
        template<typename T, class ...A>
        bool Add(A... args)
        {
            T *inst = new T(args...);

            if (inst == nullptr) {
                return false;
            }
            auto f = deleter<T>;
            m_vData.emplace(std::type_index(typeid(T)), std::make_pair(inst, f));
            return true;
        }

        // get a pointer to the type T instance inside the map
        // return nullptr if the type is not inside the map or a pointer to the type instance
        template<typename T>
        T* Get()
        {
            auto it = m_vData.find(std::type_index(typeid(T)));

            if (it == m_vData.end()) {
                return nullptr;
            }
            return static_cast<T *>(it->second.first);
        }

        // check if an instance of T is already on the map
        template<typename T>
        bool Exist()
        { return !(m_vData.find(std::type_index(typeid(T))) == m_vData.end()); }

        template<typename T, class ...A>
        bool ApplyWhile(std::function<bool(T *, A...)> f, A... args)
        {
            for (auto it = m_vData.begin(); it != m_vData.end(); ++it) {
                if (f(static_cast<T *>(it->second.first, args)...) == true) {
                    return true;
                }
            }
            return false;
        }

    };
}

#endif //FOXENGINE_TYPEDMAP_HPP
