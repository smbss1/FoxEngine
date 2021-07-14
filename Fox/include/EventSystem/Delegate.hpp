//
// Created by samuel on 14/07/2021.
//

#ifndef FOXENGINE_DELEGATE_HPP
#define FOXENGINE_DELEGATE_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <atomic>

namespace fox
{
    namespace event
    {
        class DelegateBase {};

        template <typename Signature>
        class Delegate;

        template <typename... Args>
        class Delegate<void(Args...)> : public DelegateBase
        {
            struct base
            {
                virtual ~base() {}
                virtual bool compare(base* other) = 0;
                virtual void do_call(Args... args) = 0;
            };

            template <typename T>
            struct call : base
            {
                T m_oFunc;
                template <typename S>
                call(S&& callback): m_oFunc(std::forward<S>(callback)) {}

                bool compare(base* other)
                {
                    call<T>* tmp = dynamic_cast<call<T>*>(other);
                    return tmp && m_oFunc == tmp->m_oFunc;
                }

                void do_call(Args... args)
                {
                    return m_oFunc(std::forward<Args>(args)...);
                }
            };
            std::vector<scope<base>> m_vCallbacks;

            Delegate(Delegate const&) = delete;
            void operator=(Delegate const&) = delete;
        public:
            Delegate() {}
            template <typename T>
            Delegate& operator+= (T&& callback) {
                m_vCallbacks.emplace_back(new call<T>(std::forward<T>(callback)));
                return *this;
            }
            template <typename T>
            Delegate& operator-= (T&& callback) {
                call<T> tmp(std::forward<T>(callback));
                auto it = std::remove_if(m_vCallbacks.begin(),
                                         m_vCallbacks.end(),
                                         [&](scope<base>& other) {
                                             return tmp.compare(other.get());
                                         });
                m_vCallbacks.erase(it, m_vCallbacks.end());
                return *this;
            }

            void operator()(Args... args) {
                for (auto& callback: m_vCallbacks) {
                    callback->do_call(args...);
                }
            }
        };

// ----------------------------------------------------------------------------

        template <typename RC, typename Class, typename... Args>
        class member_call {
            Class* d_object;
            RC (Class::*d_member)(Args...);
        public:
            member_call(Class* object, RC (Class::*member)(Args...))
                    : d_object(object)
                    , d_member(member) {
            }
            RC operator()(Args... args) {
                return (d_object->*d_member)(std::forward<Args>(args)...);
            }
            bool operator== (member_call const& other) const {
                return d_object == other.d_object
                       && d_member == other.d_member;
            }
            bool operator!= (member_call const& other) const {
                return !(*this == other);
            }
        };

        template <typename RC, typename Class, typename... Args>
        member_call<RC, Class, Args...> mem_call(Class& object,
                                                 RC     (Class::*member)(Args...)) {
            return member_call<RC, Class, Args...>(&object, member);
        }
    }
}

#endif //FOXENGINE_DELEGATE_HPP
