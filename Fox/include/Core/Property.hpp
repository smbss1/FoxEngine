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

#include "Core/EventNotifier.hpp"



// ===--------------------------------===
// ===---                          ---===
// ===---   FORWARD DECLARATIONS   ---===
// ===---                          ---===
// ===--------------------------------===

template<typename T>
struct basic_property_policy;

template<typename T, typename P = basic_property_policy<T>>
class basic_property;



// ===-----------------------------------------------------------------------===
// ===---                                                                 ---===
// ===---   DEFAULT PROPERTY POLICY: CONTROLS STORAGE + ACCESS + EVENTS   ---===
// ===---              NO ACCESS RESTRICTIONS, FIRES EVENTS               ---===
// ===---                                                                 ---===
// ===-----------------------------------------------------------------------===

template<typename T>
struct basic_property_policy
{
    basic_property_policy() { validate(m_value); }
    ~basic_property_policy() = default;

    basic_property_policy(const T& v) : m_value(v) { validate(m_value); }
    basic_property_policy(T&& v) : m_value(std::move(v)) { validate(m_value); }

    template<typename... A, std::enable_if_t<
            std::is_constructible_v<T, A...>>* = nullptr>
    basic_property_policy(A&&... a) : m_value(std::forward<A>(a)...) { validate(m_value); }

    template<typename V, std::enable_if_t<
            std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
    basic_property_policy(std::initializer_list<V> l) : m_value(std::move(l)) { validate(m_value); }

    using update_event_t = std::function<void(basic_property<T>*)>;

    size_t add_update_event(update_event_t proc) { return m_update_events.subscribe(proc); }
    bool remove_update_event(size_t id) { return m_update_events.unsubscribe(id); }

protected:
    template<typename U> void validate(const U& v) const {}

    decltype(auto) get() const { return(m_value); }
    decltype(auto) get() { return(m_value); }

    template<typename U> void set(const U& nv)
    {
        validate(nv);
        m_value = nv;
        fire_update_event();
    }

    template<typename U> void set(U&& nv)
    {
        validate(nv);
        m_value = std::move(nv);
        fire_update_event();
    }

private:
    void fire_update_event()
    {
        m_update_events.notifiy((basic_property<T>*)this);
    }

    T m_value = T{};
    fox::EventNotifier<void(basic_property<T>*)> m_update_events;
};



// ===-----------------------------------------------------------------------===
// ===---                                                                 ---===
// ===---   DEFAULT PROPERTY POLICY: CONTROLS STORAGE + ACCESS + EVENTS   ---===
// ===---              NO ACCESS RESTRICTIONS, FIRES EVENTS               ---===
// ===---                                                                 ---===
// ===-----------------------------------------------------------------------===

template<typename T>
struct basic_rw_property_policy
{
    using getter_type = std::function<T()>;
    using setter_type = std::function<void(T)>;

    basic_rw_property_policy(const getter_type& get, const setter_type& set) : m_getter(get), m_setter(set) { }
    basic_rw_property_policy(getter_type&& get, setter_type&& set) : m_getter(std::move(get)), m_setter(std::move(set)) { }
    basic_rw_property_policy(const basic_rw_property_policy& property_)
        : m_getter(property_.m_getter)
        , m_setter(property_.m_setter)
    { }

    ~basic_rw_property_policy() = default;

    using update_event_t = std::function<void(basic_property<T>*)>;

    size_t add_update_event(update_event_t proc) { return m_update_events.subscribe(proc); }
    bool remove_update_event(size_t id) { return m_update_events.unsubscribe(id); }

protected:
    template<typename U> void validate(const U& v) const {}

    decltype(auto) get() const { return m_getter(); }
    decltype(auto) get() { return m_getter(); }

    template<typename U> void set(const U& nv)
    {
        validate(nv);
        m_setter(nv);
        fire_update_event();
    }

    template<typename U> void set(U&& nv)
    {
        validate(nv);
        m_setter(std::move(nv));
        fire_update_event();
    }

private:
    void fire_update_event()
    {
        m_update_events.notifiy((basic_property<T>*)this);
    }

    getter_type m_getter;
    setter_type m_setter;
    fox::EventNotifier<void(basic_property<T>*)> m_update_events;
};




// ===-----------------------------------------------------------------------------===
// ===---                                                                       ---===
// ===---   READ ONLY PROPERTY POLICY: CONTROLS STORAGE + READ ACCESS           ---===
// ===---              WRITE ACCESS RESTRICTIONS                                ---===
// ===---                                                                       ---===
// ===-----------------------------------------------------------------------------===

template<typename T>
struct basic_property_read_only_policy
{
    using getter_type = std::function<T()>;

    ~basic_property_read_only_policy() = default;

    basic_property_read_only_policy(const getter_type& v) : m_getter(v) { }
    basic_property_read_only_policy(getter_type&& v) : m_getter(std::move(v)) { }
    basic_property_read_only_policy(const basic_property_read_only_policy& property_)
            : m_getter(property_.m_getter)
    { }

protected:

    decltype(auto) get() const { return m_getter(); }
    decltype(auto) get() { return m_getter(); }

    template<typename U> void set(const U& nv)
    {
        assert(false && "Cannot modified this property because is read only");
    }

    template<typename U> void set(U&& nv)
    {
        assert(false && "Cannot modified this property because is read only");
    }

private:
    getter_type m_getter = [&](){ return T(); };
};



// ===-----------------------------------------------------------------------------===
// ===---                                                                       ---===
// ===---   WRITE ONLY PROPERTY POLICY: CONTROLS STORAGE + WRITE ACCESS         ---===
// ===---              READ ACCESS RESTRICTIONS                                 ---===
// ===---                                                                       ---===
// ===-----------------------------------------------------------------------------===

template<typename T>
struct basic_property_write_only_policy
{
    using setter_type = std::function<void(T)>;

    ~basic_property_write_only_policy() = default;

    basic_property_write_only_policy(const setter_type& v) : m_setter(v) { }
    basic_property_write_only_policy(setter_type&& v) : m_setter(std::move(v)) { }

    basic_property_write_only_policy(const basic_property_write_only_policy& property_)
            : m_setter(property_.m_setter)
    { }

protected:

    template<typename U> void validate(const U& v) const {}


    decltype(auto) get() const
    {
        assert(false && "Cannot read this property because is write only");
    }

    decltype(auto) get()
    {
        assert(false && "Cannot read this property because is write only");
    }

    template<typename U> void set(const U& nv)
    {
        validate(nv);
        m_setter(nv);
        fire_update_event();
    }

    template<typename U> void set(U&& nv)
    {
        validate(nv);
        m_setter(std::move(nv));
        fire_update_event();
    }

    using update_event_t = std::function<void(basic_property<T>*)>;

    size_t add_update_event(update_event_t proc) { return m_update_events.subscribe(proc); }
    bool remove_update_event(size_t id) { return m_update_events.unsubscribe(id); }

private:
    void fire_update_event()
    {
        m_update_events.notifiy((basic_property<T>*)this);
    }

    setter_type m_setter = [&](const T& value) { };
    fox::EventNotifier<void(basic_property<T>*)> m_update_events;
};



// ===------------------------------------------------------===
// ===---                                                ---===
// ===---   STORAGE POLICY CLASS BASED PROPERTY POLICY   ---===
// ===---      NO ACCESS RESTRICTIONS, FIRES EVENTS      ---===
// ===---                                                ---===
// ===------------------------------------------------------===

template<typename T, typename P>
struct storage_property_policy
{
    storage_property_policy(const T& v, P p = P{}) : m_provider(p)
    {
        validate(v);
        save(v);
    }

    storage_property_policy(T&& v, P p = P{}) : m_provider(p)
    {
        validate(v);
        save(std::move(v));
    }

    using update_event_t = std::function<void(basic_property<T, P>*)>;

    size_t add_update_event(update_event_t proc) { return m_update_events.subscribe(proc); }
    bool remove_update_event(size_t id) { return m_update_events.unsubscribe(id); }

protected:
    void validate(const T& v) const {}

    auto get() const { return load(); }

    void set(const T& nv)
    {
        validate(nv);
        save(nv);
        fire_update_event();
    }

    void set(T&& nv)
    {
        validate(nv);
        save(std::move(nv));
        fire_update_event();
    }

private:
    void save(const T& v) { m_provider.save(v); }

    T load() const
    {
        T temp;
        m_provider.load(temp);
        return temp;
    }

    void fire_update_event()
    {
        m_update_events.notifiy((basic_property<T>*)this);
    }

    P m_provider;
    fox::EventNotifier<void(basic_property<T, P>*)> m_update_events;
};



// ===--------------------------------------------===
// ===---                                      ---===
// ===---   RAM BASED PROPERTY VALUE STORAGE   ---===
// ===---                                      ---===
// ===--------------------------------------------===

template<typename T>
struct ram_storage
{
    void save(T v) { m_value = v; }
    void load(T& v) const { v = m_value; }

private:
    T m_value = T{};
};



// ===---------------------------------------------===
// ===---                                       ---===
// ===---   DISK BASED PROPERTY VALUE STORAGE   ---===
// ===---                                       ---===
// ===---------------------------------------------===

struct disk_storage
{
    explicit disk_storage(const char* path) : m_path(path) {}

    void save(char v) { save_raw(v); }
    void load(char& v) const { load_raw(v); }

    void save(int v) { save_raw(v); }
    void load(int& v) const { load_raw(v); }

    void save(const std::string& v)
    {
        decltype(std::declval<std::string>().length()) length = v.length();
        std::ofstream ofs;
        ofs.exceptions(std::ios::failbit | std::ios::badbit);
        ofs.open(m_path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        ofs.write((const char*)&length, sizeof(length));
        ofs.write(v.c_str(), length);
        ofs.close();
    }

    void load(std::string& v) const
    {
        std::ifstream ifs;
        ifs.exceptions(std::ios::failbit | std::ios::badbit);
        ifs.open(m_path, std::ios_base::binary | std::ios_base::in);
        decltype(std::declval<std::string>().length()) length = {};
        ifs.read((char*)&length, sizeof(length));
        using buffer_t = std::vector<char>;
        buffer_t buffer(length);
        ifs.read(buffer.data(), length);
        ifs.close();
        v = std::string(std::begin(buffer), std::end(buffer));
    }

private:
    template<typename T>
    void save_raw(T v)
    {
        std::ofstream ofs;
        ofs.exceptions(std::ios::failbit | std::ios::badbit);
        ofs.open(m_path, std::ios_base::out);
        ofs.write((const char*)&v, sizeof(v));
        ofs.close();
    }

    template<typename T>
    void load_raw(T& v) const
    {
        std::ifstream ifs;
        ifs.exceptions(std::ios::failbit | std::ios::badbit);
        ifs.open(m_path);
        ifs.read((char*)&v, sizeof(v));
        ifs.close();
    }

    std::string m_path;
};



// ===--------------------------------===
// ===---                          ---===
// ===---   PROPERTY TYPE TRAITS   ---===
// ===---                          ---===
// ===--------------------------------===

template<typename>
struct is_property : std::false_type {};

template<typename T, typename P>
struct is_property<basic_property<T, P>> : std::true_type {};

template<typename T>
inline constexpr bool is_property_v = is_property<std::decay_t<T>>::value;



// ===--------------------------------------------===
// ===---                                      ---===
// ===---   PROPERTY TEMPLATE IMPLEMENTATION   ---===
// ===---                                      ---===
// ===--------------------------------------------===

template<typename T, typename P>
class basic_property : public P
{
public:
    using PT = P;

    basic_property() = default;
    ~basic_property() = default;

    basic_property(const T& v) : PT(v) {}
    basic_property(T&& v) :  PT(std::move(v)) {}

    basic_property(const basic_property& p) : PT(p.get()) {}
    basic_property(basic_property&& p) : PT(std::move(p.get())) {}

    template<typename U, typename P2> basic_property(const basic_property<U, P2>& p) : PT(p.get()) {}
    template<typename U, typename P2> basic_property(basic_property<U, P2>&& p) : PT(std::move(p.get())) {}

    template<typename... A, std::enable_if_t<
            std::is_constructible_v<PT, A...>>* = nullptr>
    basic_property(A&&... a) : PT(std::forward<A>(a)...) {}

    template<typename V, std::enable_if_t<
            std::is_constructible_v<PT, std::initializer_list<V>>>* = nullptr>
    basic_property(std::initializer_list<V> l) : PT(std::move(l)) {}

    basic_property& operator = (const T& v)
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
    basic_property& operator = (const basic_property<U, P>& p)
    {
        if(this != (decltype(this))&p)
            set(p.get());
        return *this;
    }

    template<typename U>
    basic_property& operator = (basic_property<U, P>&& p)
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

#ifdef PROPERTY_OPERATOR
    #error "PROPERTY_OPERATOR should not be defined!"
#endif
#define PROPERTY_OPERATOR(op) \
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
    PROPERTY_OPERATOR(++)
    PROPERTY_OPERATOR(--)
#undef PROPERTY_OPERATOR

#ifdef PROPERTY_OPERATOR
    #error "PROPERTY_OPERATOR should not be defined!"
#endif
#define PROPERTY_OPERATOR(op) \
	basic_property& operator op (const T& v) \
	{ \
		auto temp(get()); \
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U> \
	basic_property& operator op (const U& v) \
	{ \
		auto temp(get()); \
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	basic_property& operator op (const basic_property& p) \
	{ \
		auto temp(get()); \
		temp op p.get(); \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U> \
	basic_property& operator op (const basic_property<U, P>& p) \
	{ \
		auto temp(get()); \
		temp op p.get(); \
		set(std::move(temp)); \
		return *this; \
	}
    PROPERTY_OPERATOR(+=)
    PROPERTY_OPERATOR(-=)
    PROPERTY_OPERATOR(*=)
    PROPERTY_OPERATOR(/=)
    PROPERTY_OPERATOR(&=)
    PROPERTY_OPERATOR(|=)
    PROPERTY_OPERATOR(^=)
    PROPERTY_OPERATOR(%=)
    PROPERTY_OPERATOR(>>=)
    PROPERTY_OPERATOR(<<=)
#undef PROPERTY_OPERATOR

    decltype(auto) get() { return(PT::get()); }
    decltype(auto) get() const { return(PT::get()); }

    void set(const T& v) { PT::set(v); }
    void set(T&& v) { PT::set(std::move(v)); }

    explicit operator T& () { return get(); }
    operator const T& () const { return get(); }

    T& operator -> () { return get(); }
    const T& operator -> () const { return get(); }

    template<typename U> decltype(auto) operator [] (U&& i) { return(get()[std::forward<U>(i)]); }
    template<typename U> decltype(auto) operator [] (U&& i) const { return(get()[std::forward<U>(i)]); }

    bool operator==(const T& v) const { return get() == v; }
    bool operator!=(const T& v) const { return get() != v; }

    template<typename F, typename... A>
    auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
    { return std::invoke(std::forward<F>(f), get(), std::forward<A>(a)...); }
};



// ===-----------------------------------------===
// ===---                                   ---===
// ===---   PROPERTY ARYTHMETIC OPERATORS   ---===
// ===---                                   ---===
// ===-----------------------------------------===

#ifdef PROPERTY_OPERATOR
#error "PROPERTY_OPERATOR should not be defined!"
#endif
#define PROPERTY_OPERATOR(op) \
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
PROPERTY_OPERATOR(+)
PROPERTY_OPERATOR(-)
PROPERTY_OPERATOR(*)
PROPERTY_OPERATOR(/)
PROPERTY_OPERATOR(&)
PROPERTY_OPERATOR(|)
PROPERTY_OPERATOR(^)
PROPERTY_OPERATOR(%)
PROPERTY_OPERATOR(>>)
PROPERTY_OPERATOR(<<)
#undef PROPERTY_OPERATOR



// ===-------------------------------------------===
// ===---                                     ---===
// ===---   PROPERTY INPUT/OUTPUT OPERATORS   ---===
// ===---                                     ---===
// ===-------------------------------------------===

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



// ===-------------------------------------------===
// ===---                                     ---===
// ===---   PROPERTY MAKER HELPER FUNCTIONS   ---===
// ===---                                     ---===
// ===-------------------------------------------===

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



// ===---------------------------------===
// ===---                           ---===
// ===---   PROPERTY TYPE ALIASES   ---===
// ===---                           ---===
// ===---------------------------------===

template<typename T>
using property = basic_property<T, basic_property_policy<T>>;

template<typename T>
using rw_property = basic_property<T, basic_rw_property_policy<T>>;

template<typename T>
using read_only_property = basic_property<T, basic_property_read_only_policy<T>>;

template<typename T>
using write_only_property = basic_property<T, basic_property_write_only_policy<T>>;

template<typename T>
using ram_property = basic_property<T, storage_property_policy<T, ram_storage<T>>>;

template<typename T>
using disk_property = basic_property<T, storage_property_policy<T, disk_storage>>;

#define GET [&]()
#define SET [&](const auto& value)


// ===-------------------===
// ===---             ---===
// ===---   THE END   ---===
// ===---             ---===
// ===-------------------===

#endif //FOXENGINE_PROPERTY_HPP
