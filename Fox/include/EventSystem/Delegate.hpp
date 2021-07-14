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
        // Base class used for Template check like 'is_base_of'
        class DelegateBase {};

        //--
        //      FORWARD DECLARATION
        //--
        template <typename Signature>
        class Delegate;

        //--
        //      SIMPLE DELEGATE WITH ONLY MULTIPLE ARGUMENTS, NO RETURN TYPE
        //      RETURN: VOID
        //      ARGS: INFINITY
        //--
        template <typename... Args>
        class Delegate<void(Args...)> : public DelegateBase
        {
            struct base
            {
                virtual ~base() {}
                virtual bool compare(base* other) = 0;
                virtual void do_call(Args... args) = 0;
            };

            /**
             * @brief This class is responsible of calling the function
             * @tparam T the signature of the function
             */
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

            //--
            //      DELETED FUNCTIONS
            //--
            Delegate(Delegate const&) = delete;
            void operator=(Delegate const&) = delete;

        public:
            Delegate() = default;

            //--
            //      SUBSCRIBE OPERATOR
            //--
            template <typename T>
            Delegate& operator+= (T&& callback) {
                m_vCallbacks.emplace_back(new call<T>(std::forward<T>(callback)));
                return *this;
            }

            //--
            //      UNSUBSCRIBE OPERATOR
            //--
            template <typename T>
            Delegate& operator-= (T&& callback) {
                call<T> tmp(std::forward<T>(callback));
                auto it = std::remove_if(m_vCallbacks.begin(),
                                         m_vCallbacks.end(),
                                         [&](scope<base>& other)
                                         {
                                             return tmp.compare(other.get());
                                         });
                m_vCallbacks.erase(it, m_vCallbacks.end());
                return *this;
            }

            //--
            //      CALL OPERATOR
            //--
            void operator()(Args... args) {
                for (auto& callback: m_vCallbacks) {
                    callback->do_call(args...);
                }
            }
        }; //-- class Delegate


        /**
         * @brief This class is responsible of calling the object function
         * @tparam T the signature of the object function
         */
        template <typename RC, typename Class, typename... Args>
        class member_call
        {
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
        }; //-- member_call class

        /**
         * @brief Bind an object function to a simple function class
         * @tparam RC Return type of the function
         * @tparam Class the Class type of the object
         * @tparam Args the function arguments
         * @param object the instance
         * @param member the function member of the object
         * @return member_call class which biend the function object into a simple callabe class
         */
        template <typename RC, typename Class, typename... Args>
        member_call<RC, Class, Args...> MakeFunc(Class& object, RC (Class::*member)(Args...))
        {
            return member_call<RC, Class, Args...>(&object, member);
        }
    }
}

#endif //FOXENGINE_DELEGATE_HPP
