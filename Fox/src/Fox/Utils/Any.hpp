/*
** EPITECH PROJECT, 2021
** arcade
** File description:
** Any
*/

#ifndef UTILITY_ANY_HPP_
#define UTILITY_ANY_HPP_

#include <functional>
#include <type_traits>

class Any {
private:
    Any(void* pData, std::function<void(void*)> dtor);

public:
    Any(const Any&) = delete;
    Any(Any&&);
    ~Any();

    template <typename T, typename... Args,
        typename = std::enable_if_t<std::is_object<T>::value
            && std::is_constructible<T, Args...>::value>>
    static Any from(Args&&... args)
    {
        T* data = new T(std::forward<Args>(args)...);

        return Any(data, [](void* ptr) { delete static_cast<T*>(ptr); });
    }

    template <typename T>
    T& as()
    {
        return *static_cast<T*>(m_pData);
    }

    template <typename T>
    const T& as() const
    {
        return *static_cast<T*>(m_pData);
    }

private:
    void* m_pData = nullptr;
    std::function<void(void*)> m_dtor;
};

#endif /* !UTILITY_ANY_HPP_ */