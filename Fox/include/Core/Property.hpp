//
// Created by samuel on 11/07/2021.
//

#ifndef FOXENGINE_PROPERTY_HPP
#define FOXENGINE_PROPERTY_HPP

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>
#include <fstream>
#include <ios>
#include <cstddef>

#include "EventSystem/Delegate.hpp"
#include "Logger/Logger.hpp"

namespace fox
{
    namespace properties
    {
        //--
        //      MACROS OPERATORS
        //--
        #define PROPERTY_OPERATOR_INC(op) \
            basic_property& operator op () \
            { \
                auto temp(get()); \
                op temp; \
                set(std::move(temp)); \
                return *this; \
            } \
            basic_property operator op (int) \
            { \
                auto temp(*this); \
                operator op (); \
                return temp; \
            }

        #define PROPERTY_OPERATOR(op) \
            basic_property& operator op (const T& v) \
            { \
                auto temp(get()); \
                temp op v; \
                set(std::move(temp)); \
                return *this; \
            }                         \
            basic_property& operator op (const basic_property& p) \
            { \
                auto temp(get()); \
                temp op p.get(); \
                set(std::move(temp)); \
                return *this; \
            } \
            template <typename U, std::enable_if_t<!std::is_same_v<T, U>, int> = 0> \
            basic_property& operator op (const U& v) \
            { \
                auto temp(get()); \
                temp op v; \
                set(std::move(temp)); \
                return *this; \
            } \
            template<typename U> \
            basic_property& operator op (const basic_property<U, StoragePolicy>& p) \
            { \
                auto temp(get()); \
                temp op p.get(); \
                set(std::move(temp)); \
                return *this; \
            }

        #define PROPERTY_ARITHMETIC_OPERATOR(op) \
            template<typename T2, typename P2, typename V, std::enable_if_t<!is_property_v<V>, int> = 0> \
            auto operator op (const basic_property<T2, P2>& lhs, const V& rhs) \
                -> basic_property<decltype(std::declval<T2>() op std::declval<V>()), P2> \
            { \
                return basic_property(lhs.get() op rhs); \
            } \
            template<typename V, typename T2, typename P2, std::enable_if_t<!is_property_v<V>, int> = 0> \
            auto operator op (const V& lhs, const basic_property<T2, P2>& rhs) \
                -> basic_property<decltype(std::declval<V>() op std::declval<T2>()), P2> \
            { \
                return basic_property(lhs op rhs.get()); \
            } \
            template<typename T2, typename T3, typename P2> \
            auto operator op (const basic_property<T2, P2>& lhs, const basic_property<T3, P2>& rhs) \
                -> basic_property<decltype(std::declval<T2>() op std::declval<T3>()), P2> \
            { \
                return basic_property(lhs.get() op rhs.get()); \
            }

        //--
        //      FORWARD DECLARATION
        //--
        namespace policies
        {
            template<class T>
            class value;

            template<class T>
            class proxy;

            template<class T>
            class read_only;
        }
        template<class T, class StoragePolicy = policies::value<T>>
        class basic_property;


        //--
        //      PROPERTY TYPE TRAITS
        //--
        template<typename>
        struct is_property : std::false_type {};

        template<typename T, typename P>
        struct is_property<basic_property<T, P>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_property_v = is_property<std::decay_t<T>>::value;


        //--
        //      POLICIES
        //--
        namespace policies
        {
            //--
            //      VALUE POLICY, READ & WRITE ACCESS
            //          + AUTOMATIC DATA STORAGE
            //--
            template<class T>
            class value
            {
                using prop_type = basic_property<T, policies::value<T>>;
                // Event Type for the event OnValueChanged
                using OnValueChangedDelegate = event::Delegate<void(prop_type*)>;

            public:
                /** Default construct */
                value() {}

                /** Construct with initial property value */
                value(const T &v) : m_oData(v) {}
                value(T&& v) : m_oData(std::move(v)) {}

                /** Getter */
                T& get() { return m_oData; }
                const T& get() const { return m_oData; }

                /** Setter */
                void set(const T &v)
                {
                    m_oData = v;
                    m_oOnValueChanged((prop_type*)this);
                }

                void set(T&& v)
                {
                    m_oData = std::move(v);
                    m_oOnValueChanged((prop_type*)this);
                }

                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator += (Closure closure)
                {
                    m_oOnValueChanged += closure;
                }

                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator -= (Closure closure)
                {
                    m_oOnValueChanged -= closure;
                }

            private:
//                friend class boost::serialization::access;
//
//                template<class Archive>
//                void serialize(Archive & ar, unsigned int file_version)
//                {
//                    ar & boost::serialization::make_nvp("value", m_oData);
//                }
                T m_oData;
                OnValueChangedDelegate m_oOnValueChanged;
            };


            //--
            //      PROXY POLICY, READ & WRITE ACCESS (FROM GETTER & SETTER)
            //--
            /** Storage policy refering to external getter/setter implementations. */
            template<class T>
            class proxy
            {
                using prop_type = basic_property<T, policies::proxy<T>>;
                // Event Type for the event OnValueChanged
                using OnValueChangedDelegate = event::Delegate<void(prop_type*)>;

            public:
                /** Prototype of getter function */
                using get_fnc_type = std::function<T(void)>;
                /** Prototype of setter function */
                using set_fnc_type = std::function<void(const T&)>;

                /** Default constructor */
                proxy()
                        : m_getter(&proxy<T>::default_get),
                          m_setter(&proxy<T>::default_set)
                {}

                /** Construct from bindings */
                proxy(const get_fnc_type &get_fnc, const set_fnc_type &set_fnc)
                        : m_getter(get_fnc),
                          m_setter(set_fnc)
                {}

                /** Bind getter to proxy */
                void bind_get(const get_fnc_type &f)
                { m_getter = f; }

                /** Bind setter to proxy */
                void bind_set(const set_fnc_type &f)
                { m_setter = f; }

                /** Getter */
                auto get()
                { return m_getter(); }
                const T& get() const
                { return m_getter(); }

                /** Setter */
                void set(const T &v)
                {
                    if (v != get()) {
                        m_setter(v);
                        m_oOnValueChanged((prop_type *) this);
                    }
                }

                void set(T&& v)
                {
                    if (v != get()) {
                        m_setter(std::move(v));
                        m_oOnValueChanged((prop_type *) this);
                    }
                }


                //--
                //      OPERATORS FOR EVENTS
                //--
                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator += (Closure closure)
                {
                    m_oOnValueChanged += closure;
                }

                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator -= (Closure closure)
                {
                    m_oOnValueChanged -= closure;
                }


            private:
                static T default_get() { throw std::runtime_error("Proxy getter not bound"); }
                static void default_set(const T &) { throw std::runtime_error("Proxy setter not bound"); }

                // not serializable

                get_fnc_type m_getter;
                set_fnc_type m_setter;

                OnValueChangedDelegate m_oOnValueChanged;
            };


            //--
            //      READONLY POLICY, READ ACCESS ONLY (FROM GETTER)
            //--
            /** Storage policy refering to external getter implementations. */
            template<class T>
            class read_only
            {
            public:
                /** Prototype of getter function */
                using get_fnc_type = std::function<const T&(void)>;

                /** Default constructor */
                read_only()
                        : m_getter(&read_only<T>::default_get)
                {}

                /** Construct from bindings */
                read_only(const get_fnc_type &get_fnc)
                        : m_getter(get_fnc)
                {}

                /** Bind getter to read_only */
                void bind_get(const get_fnc_type &f)
                { m_getter = f; }

                /** Getter */
                T& get()
                { return m_getter(); }
                const T& get() const
                { return m_getter(); }


                //--
                //      OPERATORS FOR EVENTS
                //--
                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator += (Closure closure)
                {
                }

                template<typename Closure,
                        std::enable_if_t<!std::is_base_of_v<event::DelegateBase, std::decay_t<Closure>>, int> = 0>
                void operator -= (Closure closure)
                {
                }


            private:
                static T default_get() { throw std::runtime_error("Read Only getter not bound"); }

                // not serializable

                get_fnc_type m_getter;
            };
        }


        /** Read/write policy based property */
        template<typename T, typename StoragePolicy>
        class basic_property : public StoragePolicy
        {
            using SP = StoragePolicy;

        public:
            /** Default constructor */
            basic_property() = default;

            /** Default destructor */
            ~basic_property() = default;

//            basic_property(const T& v) : SP(v) {}
//            basic_property(T&& v) :  SP(std::move(v)) {}

            //--
            //      COPY CONSTRUCTOR
            //--
            basic_property(const basic_property& p) : SP(p.get()) {}
            template<typename U, typename P2>
            basic_property(const basic_property<U, P2>& p) : SP(p.get()) {}

            //--
            //      MOVE CONSTRUCTOR
            //--
            basic_property(basic_property&& p) : SP(std::move(p.get())) {}
            template<typename U, typename P2>
            basic_property(basic_property<U, P2>&& p) : SP(std::move(p.get())) {}

            //--
            //      TEMPLATE ARGS CONSTRUCTOR
            //--
            template<typename... A, std::enable_if_t<
                    std::is_constructible_v<SP, A...>>* = nullptr>
            basic_property(A&&... a) : SP(std::forward<A>(a)...) {}

            template<typename V, std::enable_if_t<
                    std::is_constructible_v<SP, std::initializer_list<V>>>* = nullptr>
            basic_property(std::initializer_list<V> l) : SP(std::move(l)) {}

            //--
            //      GETTER
            //--
            decltype(auto) get() const
            { return SP::get(); }

            decltype(auto) get()
            { return SP::get(); }

            decltype(auto) operator()()
            { return get(); }

            decltype(auto) operator()() const
            { return get(); }


            //--
            //      SETTER
            //--
            void set(const T& v)
            { SP::set(v); }

            void set(T&& v)
            { SP::set(std::move(v)); }

            void operator()(const T &v)
            { set(v); }

            void operator()(T&& v)
            { set(std::move(v)); }


            //--
            //      ASSIGNMENT OPERATORS
            //--
            basic_property& operator=(const T& v)
            {
                set(v);
                return *this;
            }

            basic_property& operator = (T&& v)
            {
                set(std::move(v));
                return *this;
            }

            template<typename U>
                std::enable_if_t<!is_property_v<U>, basic_property&>
            operator = (const U& v)
            {
                set(v);
                return *this;
            }

            template<typename U>
                std::enable_if_t<!is_property_v<U>, basic_property&>
            operator = (U&& v)
            {
                set(std::forward<U>(v));
                return *this;
            }

            basic_property& operator = (const basic_property& p)
            {
                if(this != &p)
                    set(p.get());
                return *this;
            }

            basic_property& operator = (basic_property&& p)
            {
                if(this != &p)
                    set(std::move(p.get()));
                return *this;
            }

            template<typename U>
            basic_property& operator = (const basic_property<U, StoragePolicy>& p)
            {
                if(this != (decltype(this))&p)
                    set(p.get());
                return *this;
            }

            template<typename U>
            basic_property& operator = (basic_property<U, StoragePolicy>&& p)
            {
                if(this != (decltype(this))&p)
                    set(std::move(p.get()));
                return *this;
            }

            template<typename U, typename V>
            basic_property& operator = (const basic_property<U, V>& p)
            {
                if(this != (decltype(this))&p)
                    set(p.get());
                return *this;
            }

            template<typename U, typename V>
            basic_property& operator = (basic_property<U, V>&& p)
            {
                if(this != (decltype(this))&p)
                    set(std::move(p.get()));
                return *this;
            }

            //--
            //      ARYTHMETIC OPERATORS
            //--
            PROPERTY_OPERATOR_INC(++)
            PROPERTY_OPERATOR_INC(--)
//            PROPERTY_OPERATOR(+=)
//            PROPERTY_OPERATOR(-=)
            PROPERTY_OPERATOR(*=)
            PROPERTY_OPERATOR(/=)

            //--
            //      BITS OPERATORS
            //--
            PROPERTY_OPERATOR(&=)
            PROPERTY_OPERATOR(|=)
            PROPERTY_OPERATOR(^=)
            PROPERTY_OPERATOR(%=)
            PROPERTY_OPERATOR(>>=)
            PROPERTY_OPERATOR(<<=)

            operator T& () { return get(); }
            operator const T& () const { return get(); }

            T& operator -> () { return get(); }
            const T& operator -> () const { return get(); }

            //-- ARRAY --//
            template<typename U>
            decltype(auto) operator [] (U&& i) { return get()[std::forward<U>(i)]; }
            template<typename U>
            decltype(auto) operator [] (U&& i) const { return get()[std::forward<U>(i)]; }

            //-- EQUALITY --//
            bool operator==(const T& v) const { return get() == v; }
            bool operator!=(const T& v) const { return get() != v; }

            //-- ITERATOR --//
            decltype(auto) begin()
            { return get().begin(); }

            decltype(auto) end()
            { return get().begin(); }

            decltype(auto) begin() const
            { return get().begin(); }

            decltype(auto) end() const
            { return get().begin(); }

            template<typename Closure>
            basic_property& operator += (Closure closure)
            {
                SP::operator += (closure);
                return *this;
            }

            template<typename Closure>
            basic_property& operator -= (Closure closure)
            {
                SP::operator -= (closure);
                return *this;
            }

        private:
//            friend class boost::serialization::access;
//
//            template<class Archive>
//            void serialize(Archive &ar, unsigned int file_version)
//            {
//                ar & boost::serialization::make_nvp("storage", _s);
//            }
        };
        //-- class basic_property

        //--
        //      PROPERTY ARYTHMETIC OPERATORS
        //--
        PROPERTY_ARITHMETIC_OPERATOR(+)
        PROPERTY_ARITHMETIC_OPERATOR(-)
        PROPERTY_ARITHMETIC_OPERATOR(*)
        PROPERTY_ARITHMETIC_OPERATOR(/)
        PROPERTY_ARITHMETIC_OPERATOR(&)
        PROPERTY_ARITHMETIC_OPERATOR(|)
        PROPERTY_ARITHMETIC_OPERATOR(^)
        PROPERTY_ARITHMETIC_OPERATOR(%)
        PROPERTY_ARITHMETIC_OPERATOR(>>)
        PROPERTY_ARITHMETIC_OPERATOR(<<)

        //--
        //      PROPERTY STREAM OPERATORS
        //--
        template<typename T, typename P>
        inline std::istream& operator >> (std::istream& is, basic_property<T, P>& p)
        {
            is >> p.get();
            return is;
        }

        template<typename T, typename P>
        inline std::ostream& operator << (std::ostream& os, const basic_property<T, P>& p)
        {
            os << p.get();
            return os;
        }


        //--
        //      PROPERTY MAKER HELPER FUNCTIONS
        //--
        template<typename T, typename V>
        inline auto make_property(std::initializer_list<V> l)
        {
            using U = std::decay_t<T>;
            return basic_property<T>(U(l));
        }

        template<typename T, typename P, typename V>
        inline auto make_property(std::initializer_list<V> l)
        {
            using U = std::decay_t<T>;
            return basic_property<T, P>(U(l));
        }

        template<typename T, typename... A>
        inline auto make_property(A&&... a)
        {
            using U = std::decay_t<T>;
            return basic_property<T>(U(std::forward<A>(a)...));
        }

        template<typename T, typename P, typename... A>
        inline auto make_property(A&&... a)
        {
            using U = std::decay_t<T>;
            return basic_property<T, P>(U(std::forward<A>(a)...));
        }


        //--
        //      PROPERTY TYPE ALIASES
        //--
        template<typename T>
        using property = basic_property<T, properties::policies::value<T>>;

        template<typename T>
        using rw_property = basic_property<T, properties::policies::proxy<T>>;

        template<typename T>
        using read_property = basic_property<T, properties::policies::read_only<T>>;


        #define GET [&](auto&&... args) -> decltype(auto)
        #define SET [&](const auto& value)

        // Undef the macro to not have problems with other libraries
        #undef PROPERTY_OPERATOR
        #undef PROPERTY_OPERATOR_INC
        #undef PROPERTY_ARITHMETIC_OPERATOR
    }
}

#endif //FOXENGINE_PROPERTY_HPP
