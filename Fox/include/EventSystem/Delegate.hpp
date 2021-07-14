//
// Created by samuel on 14/07/2021.
//

#ifndef FOXENGINE_DELEGATE_HPP
#define FOXENGINE_DELEGATE_HPP

#include <atomic>

namespace fox
{
    namespace event
    {
        using DelegateUID = unsigned long long;

        static std::atomic<DelegateUID> delegate_uuid{ 1 };

        //
        // For generic types that are functors, delegate to its 'operator()'
        template <typename T>
        struct function_traits
                : public function_traits<decltype(&T::operator())> {};

        // for pointers to member function
        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits<ReturnType(ClassType::*)(Args...) const> {
            enum { arity = sizeof...(Args) };
            using f_type = std::function<ReturnType(Args...)>;

            using result = ReturnType;
            using parameters = std::tuple<Args...>;
        };

        // for pointers to member function
        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits<ReturnType(ClassType::*)(Args...) > {
            enum { arity = sizeof...(Args) };
            using f_type = std::function<ReturnType(Args...)>;

            using result = ReturnType;
            using parameters = std::tuple<Args...>;
        };

        // for function pointers
        template <typename ReturnType, typename... Args>
        struct function_traits<ReturnType (*)(Args...)>  {
            enum { arity = sizeof...(Args) };
            using f_type = std::function<ReturnType(Args...)>;

            using result = ReturnType;
            using parameters = std::tuple<Args...>;
        };

        // Alias for easy to use
        template<typename F>
        using function_type_t = typename function_traits<F>::f_type;

        template<typename F>
        using function_result_t = typename function_traits<F>::result;

        template<typename F>
        using function_arguments_t = typename function_traits<F>::parameters;

        template<std::size_t N, typename F>
        using nth_argument_t = std::tuple_element_t<N, function_arguments_t<F>>;

        template<typename F>
        constexpr auto arguments_count = std::tuple_size<function_arguments_t<F>>::value;


        //


        // pass by copy
        class DelegateTag
        {
            DelegateUID tag;

        protected:
            DelegateTag(std::nullptr_t) : tag(0) {}
        public:
            DelegateTag() : tag(delegate_uuid.fetch_add(1)) {}

            bool operator==(const DelegateTag& other) const {
                return tag == other.tag;
            }
            bool operator!=(const DelegateTag& other) const {
                return tag != other.tag;
            }
        };

        namespace details
        {
            class DelegateTagEmpty : public DelegateTag {
                using DelegateTag::DelegateTag;
            public:
                DelegateTagEmpty() : DelegateTag(nullptr) {};
                using DelegateTag::operator==;
                using DelegateTag::operator!=;
            };

            class DelegateBase {};
        }

        struct DelegateBase
        {
        public:
            auto &tag()
            { return m_oTag; }

        private:
            DelegateTag m_oTag;
        };

        template<typename Function, typename Tuple, size_t ... I>
        auto call(Function f, Tuple t, std::index_sequence<I ...>)
        {
            return f(std::get<I>(t) ...);
        }

        template<typename Function, typename Tuple>
        auto call(Function f, Tuple t)
        {
            static constexpr auto size = std::tuple_size<Tuple>::value;
            return call(f, t, std::make_index_sequence<size>{});
        }

        template<typename signature>
        class Delegate;
//
//        template <typename T>
//        struct Func : public Func<decltype(&T::operator())>
//        {
//        };
//
//        template <typename ReturnType, typename... Args>
//        struct Func<ReturnType (*)(Args...)>
//        {
//            using FuncType = ReturnType (*)(Args...);
//
//            ReturnType operator()(Args... args) const
//            {
//                return (*m_oFunc)(args...);
//            }
//
//            FuncType m_oFunc;
//        };
//
//        template <typename ClassType, typename ReturnType, typename... Args>
//        struct Func<ReturnType(ClassType::*)(Args...) const>
//        {
//            using FuncType = ReturnType (*)(Args...);
//
//            ReturnType operator()(Args... args) const
//            {
//                return (m_oCallee.*m_oFunc)(args...);
//            }
//
//            const ClassType& m_oCallee;
//            FuncType m_oFunc;
//        };
//
//        template <typename ClassType, typename ReturnType, typename... Args>
//        struct Func<ReturnType(ClassType::*)(Args...)>
//        {
//        };


        template <size_t index>
        struct next_index
        {
            static const size_t value = index - 1;
        };

        template <>
        struct next_index<0>
        {
            static const size_t value = 0;
        };

        template <class Tuple, size_t index>
        bool is_same(const Tuple& left, const Tuple& right)
        {
            if (index != 0)
                return is_same<Tuple, next_index<index>::value>(left, right) and std::get<index>(left) != std::get<index>(right);
            return std::get<index>(left) != std::get<index>(right);
        }

        template <typename Tuple>
        constexpr bool IsSame(const Tuple& left, const Tuple& right)
        {
            return is_same<Tuple, std::tuple_size<Tuple>::value - 1>(left, right);
        }

        /**
         * specialization for member functions
         *
         * \tparam T            class-type of the object who's member function to call
         * \tparam R            return type of the function that gets captured
         * \tparam params       variadic template list for possible arguments
         *                      of the captured function
         */
        template<typename T>
        class Delegate : public DelegateBase
        {
//            using params_type = decltype(function_arguments_t<T>());
//            using func_params = function_arguments_t<T>;
        public:

            Delegate() = default;

            Delegate(function_type_t<T> func) : m_oFunc(func)
            {}

            bool operator==(const Delegate &other) const
            {
                return (m_oFunc == other.m_oFunc);
            }

            template<typename... Args>
            function_result_t<T> operator()(Args... args) const
            {
                return m_oFunc(args...);
            }

            bool operator!=(const Delegate &other) const
            {
                return !((*this) == other);
            }

//            template<typename L, std::size_t... S>
//            struct Func
//            {
//            public:
//                constexpr Func(L l) noexcept : m_oFunc(std::move(l)) {}
//                auto operator() (nth_argument_t<S, L>... args) const -> function_result_t<L>
//                {
//                    return lambda(std::forward<nth_argument_t<S, L>>(args)...);
//                }
//
//            private:
//                function_type_t<L> m_oFunc;
//            };

        private:
//            template <std::size_t ... Is, typename Tuple1, typename Tuple2>
//            constexpr bool IsSame(std::index_sequence<Is...>, const Tuple1& left, const Tuple2& right)
//            {
//                return ((std::get<Is>(left) == std::get<Is>(right)) && ...);
//            }

//            Func<T, std::make_index_sequence<arguments_count<T>>()> m_oFunc;
            function_type_t<T> m_oFunc;
        };

//        /**
//         * specialization for const member functions
//         */
//        template<typename T, typename R, typename... Params>
//        class Delegate<R (T::*)(Params...) const> : public DelegateBase
//        {
//        public:
//            typedef R (T::*func_type)(Params...) const;
//
//            Delegate() = default;
//
//            Delegate(func_type func, const T &callee)
//                    : m_oCallee(callee), m_oFunc(func)
//            {}
//
//            R operator()(Params... args) const
//            {
//                return (m_oCallee.*m_oFunc)(args...);
//            }
//
//            bool operator==(const Delegate &other) const
//            {
//                return (&m_oCallee == &other.m_oCallee) && (m_oFunc == other.m_oFunc);
//            }
//
//            bool operator!=(const Delegate &other) const
//            {
//                return !(*this == other);
//            }
//
//        private:
//            const T &m_oCallee;
//            func_type m_oFunc;
//        };
//
//        /**
//         * specialization for free functions
//         *
//         * \tparam R            return type of the function that gets captured
//         * \tparam params       variadic template list for possible arguments
//         *                      of the captured function
//         */
//        template<typename R, typename... Params>
//        class Delegate<R (*)(Params...)> : public DelegateBase
//        {
//        public:
//            typedef R (*func_type)(Params...);
//
//            Delegate() = default;
//
//            Delegate(func_type func) : m_oFunc(func) {}
//
//            R operator()(Params... args) const
//            {
//                return (*m_oFunc)(args...);
//            }
//
//            bool operator==(const Delegate &other) const
//            {
//                return m_oFunc == other.m_oFunc;
//            }
//
//            bool operator!=(const Delegate &other) const
//            {
//                return !((*this) == other);
//            }
//
//        private:
//            func_type m_oFunc;
//        };

        /**
         * function to deduce template parameters from call-context
         */
//        template<typename F, typename T>
//        Delegate<F> MakeDelegate(F func, T &obj)
//        {
//            return Delegate<F>(func, obj);
//        }
        template<typename signature>
        Delegate<signature> MakeDelegate(signature func);

        template<typename R, typename... Args>
        auto MakeDelegate(R (*func)(Args...)) -> Delegate<R(Args...)>
        {
            return { func };
        }

        template<typename R, typename... Args, typename ClassType>
        auto MakeDelegate(R(ClassType::*func)(Args...)) -> Delegate<R(Args...)>
        {
            return { func };
        }

        template<typename R, typename... Args, class T>
        auto MakeDelegate(T&& func) -> Delegate<decltype(R (func(std::declval<Args>()...))) (Args...)>
        {
            return { func };
        }

        //handles explicit overloads
        template<typename ReturnType, typename... Args, typename ClassType>
        auto make_function(ReturnType(ClassType::*p)(Args...))
            -> std::function<ReturnType(Args...)> {
            return { p };
        }
    }
}

#endif //FOXENGINE_DELEGATE_HPP
