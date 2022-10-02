//
// Created by samuel on 05/07/2021.
//

#ifndef FOXENGINE_EVENTSYSTEM_HPP
#define FOXENGINE_EVENTSYSTEM_HPP

#include <map>
#include <typeinfo>
#include <iostream>
#include <mutex>
#include "common.hpp"
#include "TypedMap.hpp"

namespace fox
{
    namespace event
    {
        // internal event listener storage
        template<typename E>
        struct Event
        {
            // the listeners
            std::vector <std::function<void(const E &)>> m_vListeners;

            // dispatch the event to the listeners
            void Emit(const E &ev)
            {
                for (auto listener : m_vListeners) {
                    listener(ev);
                }
            }

            // add a new listener to the internal vector of listener for this event
            void Add(std::function<void(const E &)> fn)
            {
                m_vListeners.push_back(fn);
            }

            // get the listeners list
            const std::vector <std::function<void(const E &)>> &Listeners() const
            {
                return m_vListeners;
            }
        };

        class EventSystem
        {
        private:
            // list of listeners for all events
//            std::vector<std::function<void(const any_event&)>> all_events_listeners;

            // typed_map containing the events lists
            // this ensure that only one producer exist for a given event
            typed_map m_vEvents;

            // allow us to use the event in an asynchronous way
//            std::mutex access_mtx;
//            static EventMap m_vEvents;

            static scope<EventSystem> m_pInstance;

        public:

            static EventSystem& Get()
            {
                if (!m_pInstance)
                    m_pInstance = new_scope<EventSystem>();
                return *m_pInstance;
            }

            template<typename E>
            bool AddEvent()
            {
                // std::lock_guard <std::mutex> lock(this->access_mtx);

                if (m_vEvents.Exist<Event<E>>()) {
                    return false;
                }

                m_vEvents.Add<Event<E>>();
                return true;
            }

            // add a listener to an existing event.
            // the function take an std::function parameterized with the type of the
            // event, e.g for an event `custom_event`, the parameter of add_listener
            // should be of type std::function<void(const custom_event&)>
            // if the event exist true true is return, false otherwise
            template<typename E>
            bool On(std::function<void(const E &)> fn)
            {
                // std::lock_guard<std::mutex> lock(this->access_mtx);

                if (!m_vEvents.Exist<Event<E>>()) {
                    AddEvent<E>();
                }

                m_vEvents.Get<Event<E>>()->Add(fn);
                return true;
            }
//
//            bool listen_any(std::function<void(const any_event&)> fn) {
//                std::lock_guard<std::mutex> lock(this->access_mtx);
//
//                this->all_events_listeners.push_back(fn);
//
//                return true;
//            }

            // trigger a new event
            // the event must be previously registered with `add_event`
            // the function check if the event is register the event is
            // dispatched to the producer<E> and true is returned, otherwise
            // false is returned
            template<typename E>
            bool Emit(const E &ev)
            {
                // std::lock_guard<std::mutex> lock(this->access_mtx);

                if (!m_vEvents.Exist<Event<E>>()) {
                    return false;
                }

                // notify listener of the given event
                m_vEvents.Get<Event<E>>()->Emit(ev);

                // notify listeners of all events
//                if (!this->all_events_listeners.empty()) {
//                    auto any_ev = any_event(ev);
//                    for (auto al : this->all_events_listeners) {
//                        al(any_ev);
//                    }
//                }
                return true;
            }

            // cleanup all listeners before exit
            void cleanup();
        };
    }
}

#endif //FOXENGINE_EVENTSYSTEM_HPP
