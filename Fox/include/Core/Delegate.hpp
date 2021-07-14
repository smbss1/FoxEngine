//
// Created by samuel on 14/07/2021.
//

#ifndef FOXENGINE_DELEGATE_HPP
#define FOXENGINE_DELEGATE_HPP

namespace fox
{
    template<typename signature>
    class Delegate;

    /**
     * specialization for member functions
     *
     * \tparam T            class-type of the object who's member function to call
     * \tparam R            return type of the function that gets captured
     * \tparam params       variadic template list for possible arguments
     *                      of the captured function
     */
    template <typename T, typename R, typename... Params>
    class Delegate<R (T::*)(Params...)>
    {
    public:
        typedef R (T::*func_type)(Params...);

        Delegate(func_type func, T& callee)
                : m_oCallee(callee)
                , m_oFunc(func)
        {}

        R operator()(Params... args) const
        {
            return (m_oCallee.*m_oFunc)(args...);
        }

        bool operator==(const Delegate& other) const
        {
            return (&m_oCallee == &other.m_oCallee) && (m_oFunc == other.m_oFunc);
        }
        bool operator!= (const Delegate& other) const
        {
            return !((*this) == other);
        }

    private:
        T& m_oCallee;
        func_type m_oFunc;
    };

    /**
     * specialization for const member functions
     */
    template <typename T, typename R, typename... Params>
    class Delegate<R (T::*)(Params...) const>
    {
    public:
        typedef R (T::*func_type)(Params...) const;

        Delegate(func_type func, const T& callee)
                : m_oCallee(callee)
                , m_oFunc(func)
        {}

        R operator()(Params... args) const
        {
            return (m_oCallee.*m_oFunc)(args...);
        }

        bool operator==(const Delegate& other) const
        {
            return (&m_oCallee == &other.m_oCallee) && (m_oFunc == other.m_oFunc);
        }
        bool operator!= (const Delegate& other) const
        {
            return !(*this == other);
        }

    private:
        const T& m_oCallee;
        func_type m_oFunc;
    };

/**
 * specialization for free functions
 *
 * \tparam R            return type of the function that gets captured
 * \tparam params       variadic template list for possible arguments
 *                      of the captured function
 */
    template <typename R, typename... Params>
    class Delegate<R (*)(Params...)>
    {
    public:
        typedef R (*func_type)(Params...);

        Delegate(func_type func) : m_oFunc(func) {}

        R operator()(Params... args) const
        {
            return (*m_oFunc)(args...);
        }

        bool operator==(const Delegate& other) const
        {
            return m_oFunc == other.m_oFunc;
        }

        bool operator!= (const Delegate& other) const
        {
            return !((*this) == other);
        }

    private:
        func_type m_oFunc;
    };

    /**
     * function to deduce template parameters from call-context
     */
    template <typename F, typename T>
    Delegate<F> MakeDelegate(F func, T& obj)
    {
        return Delegate<F>(func, obj);
    }

    template <typename T>
    Delegate<T> MakeDelegate(T func)
    {
        return Delegate<T>(func);
    }
}

#endif //FOXENGINE_DELEGATE_HPP
