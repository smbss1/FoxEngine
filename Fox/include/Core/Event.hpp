/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Event.hpp
*/

#ifndef FOX_EVENT_HPP_
#define FOX_EVENT_HPP_

#include <list>
#include <memory>
#include <functional>
#include <algorithm>


namespace fox
{
    template <typename signature>
    class EventNotifier;

    template <class R, class... Args>
    class EventNotifier<R (Args...)>
    {
        using CbFunc = std::function<R (Args...)>;
        using CallbackSlot = std::shared_ptr<CbFunc>;
        using CallbackList = std::list<CallbackSlot>;

        CallbackList m_vCallbacks;
    public:
        /**
         * @brief Construct a new Event Notifier object
         */
        EventNotifier() { }
        EventNotifier(const EventNotifier& other) : m_vCallbacks(other.m_vCallbacks) { }

        /**
         * @brief Destroy the Event Notifier object
         */
        ~EventNotifier() = default;

        /**
         * @brief Add a new subsciber
         *
         * @param cb the callback to add
         * @return size_t the id of the subscriber
         */
        size_t subscribe(const CbFunc& cb)
        {
            m_vCallbacks.emplace_back(std::make_shared<CbFunc>(cb));
            return size_t (m_vCallbacks.back().get());
        }

        /**
         * @brief Remove a subscribers to the list
         *
         * @param id the id of the subscriber
         * @return true if the subscriber successly removed
         * @return false if it's failed
         */
        bool unsubscribe(size_t id)
        {
            auto it = std::remove_if(m_vCallbacks.begin(), m_vCallbacks.end(),
                                    [id](const CallbackSlot& slot) {
                                        return size_t(slot.get()) == id;
                                    });
            bool const removed = it != m_vCallbacks.end();
            m_vCallbacks.erase(it);
            return removed;
        }

        /**
         * @brief Notify the subscribers
         *
         * @param args the arguments to pass to the subscribers
         */
        void notifiy(Args... args)
        {
            for (auto& slot : m_vCallbacks) {
                if (slot) {
                    (*slot)(args...);
                }
            }
        }

        //
        /**
         * @brief Get the number of connected subscibers.
         *
         * @return std::size_t the number of connected subscibers.
         */
        std::size_t size () const
        {
            return m_vCallbacks.size();
        }

    private:
    };

    /// This function creates a std::function by binding @a object to the member function pointer @a method.
    template<class Instance, class C, class R, class... Args>
    std::function<R (Args...)> slot (Instance &object, R (C::*method) (Args...))
    {
        return [&object, method] (Args... args)
        {
            return (object.*method) (args...);
        };
    }

    /// This function creates a std::function by binding @a object to the member function pointer @a method.
    template<class C, class R, class... Args>
    std::function<R (Args...)> slot (C *object, R (C::*method) (Args...))
    {
        return [object, method] (Args... args)
        {
            return (object->*method) (args...);
        };
    }

} // namespace fox

#endif