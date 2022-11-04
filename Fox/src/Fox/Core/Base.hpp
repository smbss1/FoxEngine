//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_BASE_HPP
#define FOXENGINE_BASE_HPP

#define FOX_EXPAND_MACRO(x) x
#define FOX_STRINGIFY_MACRO(x) #x
#define BIT(x) (1 << x)

#define FOX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template <typename F>
struct privDefer {
    F f;
    privDefer(F f) : f(f) {}
    ~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
    return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})

//// Properties
//// a read-write property with data store and
//// automatically generated get/set functions.
//// this is what C++/CLI calls a trivial scalar
//// property
//template <class T>
//class Property
//{
//    T data;
//public:
//    // might be useful for template
//    // deductions
//    using value_type = T;
//
//    // access with function call syntax
//    Property() : data() { }
//    T operator()() const
//    {
//        return data;
//    }
//
//    T operator()(T const & value)
//    {
//        data = value;
//        return data;
//    }
//
//    // access with get()/set() syntax
//    T get() const
//    {
//        return data;
//    }
//
//    T set(T const & value)
//    {
//        data = value;
//        return data;
//    }
//
//    // access with '=' sign
//    // in an industrial-strength library,
//    // specializations for appropriate types
//    // might choose to add combined operators
//    // like +=, etc.
//    operator T() const
//    {
//        return data;
//    }
//
//    T operator=(T const & value)
//    {
//        data = value;
//        return data;
//    }
//
//    bool operator==(T const & value) const {
//        return data == value;
//    }
//};
//
//// a read-only property calling a
//// user-defined getter
//template <typename T, typename Object,
//        T (Object::*real_getter)()>
//class ROProperty {
//    Object * my_object;
//public:
//    ROProperty() : my_object(0) {}
//    ROProperty(Object * me = 0)
//            : my_object(me) {}
//
//    // this function must be called by the
//    // containing class, normally in a
//    // constructor, to initialize the
//    // ROProperty so it knows where its
//    // real implementation code can be
//    // found.
//    // obj is usually the containing
//    // class, but need not be; it could be a
//    // special implementation object.
//    void operator()(Object * obj) {
//        my_object = obj;
//    }
//
//    // function call syntax
//    T operator()() const {
//        return (my_object->*real_getter)();
//    }
//
//    // get/set syntax
//    T get() const {
//        return (my_object->*real_getter)();
//    }
//    void set(T const & value);
//    // reserved but not implemented,
//    // per C++/CLI
//
//    // use on rhs of '='
//    operator T() const {
//        return (my_object->*real_getter)();
//    }
//
//    typedef T value_type;
//    // might be useful for template
//    // deductions
//};
//
//// a write-only property calling a
//// user-defined setter
//template <class T, class Object,
//        T (Object::*real_setter)(T const &)>
//class WOProperty {
//    Object * my_object;
//public:
//    WOProperty() : my_object(0) {}
//    WOProperty(Object * me = 0)
//            : my_object(me) {}
//
//    // this function must be called by the
//    // containing class, normally in a
//    // constructor, to initialize the
//    // WOProperty so it knows where its real
//    // implementation code can be found
//    void operator()(Object * obj) {
//        my_object = obj;
//    }
//    // function call syntax
//    T operator()(T const & value) {
//        return (my_object->*real_setter)(value);
//    }
//    // get/set syntax
//    T get() const;
//    // reserved but not implemented,
//    // per C++/CLI
//    T set(T const & value) {
//        return (my_object->*real_setter)(value);
//    }
//
//    // access with '=' sign
//    T operator=(T const & value) {
//        return (my_object->*real_setter)(value);
//    }
//
//    typedef T value_type;
//    // might be useful for template
//    // deductions
//};
//
//// a read-write property which invokes
//// user-defined functions
//template <class T,
//        class Object,
//        T (Object::*real_getter)(),
//        void (Object::*real_setter)(T const &)>
//class RWProperty
//{
//    Object * my_object;
//public:
//    RWProperty() : my_object(0) {}
//    RWProperty(Object * me = 0)
//            : my_object(me) {}
//
//    // this function must be called by the
//    // containing class, normally in a
//    // constructor, to initialize the
//    // ROProperty so it knows where its
//    // real implementation code can be
//    // found
//    void operator()(Object * obj) {
//        my_object = obj;
//    }
//
//    // function call syntax
//    T operator()() const {
//        return (my_object->*real_getter)();
//    }
//    void operator()(T const & value) {
//        (my_object->*real_setter)(value);
//    }
//
//    // get/set syntax
//    T get() const {
//        return (my_object->*real_getter)();
//    }
//    void set(T const & value) {
//        (my_object->*real_setter)(value);
//    }
//    // access with '=' sign
//    operator T() const {
//        return (my_object->*real_getter)();
//    }
//    void operator=(T const & value) {
//        (my_object->*real_setter)(value);
//    }
//    bool operator==(T const & value) const {
//        return (my_object->*real_getter)() == value;
//    }
//
//    typedef T value_type;
//    // might be useful for template
//    // deductions
//};
//
//#include <map>
//template <class KeyCode,
//        class T,
//        class Compare = std::less<KeyCode>,
//        class Allocator = std::allocator<std::pair<const KeyCode, T>>>
//class IndexedProperty
//{
//    std::map<KeyCode, T, Compare,
//            Allocator> data;
//    typedef typename std::map<KeyCode, T, Compare,
//            Allocator>::iterator
//            map_iterator;
//public:
//
//    // function call syntax
//    T operator()(KeyCode const & key) {
//        std::pair<map_iterator, bool> result;
//        result
//                = data.insert(std::make_pair(key, T()));
//        return (*result.first).second;
//    }
//    T operator()(KeyCode const & key,
//                 T const & t) {
//        std::pair<map_iterator, bool> result;
//        result
//                = data.insert(std::make_pair(key, t));
//        return (*result.first).second;
//    }
//
//    // get/set syntax
//    T get_Item(KeyCode const & key) {
//        std::pair<map_iterator, bool> result;
//        result
//                = data.insert(std::make_pair(key, T()));
//        return (*result.first).second;
//    }
//    T set_Item(KeyCode const & key,
//               T const & t) {
//        std::pair<map_iterator, bool> result;
//        result
//                = data.insert(std::make_pair(key, t));
//        return (*result.first).second;
//    }
//
//    // operator [] syntax
//    T& operator[](KeyCode const & key) {
//        return (*((data.insert(make_pair(
//                key, T()))).first)).second;
//    }
//};
//
//#define PROP(TYPE) Property<TYPE>
//#define ROPROP(TYPE, CLASS, GETTER) ROProperty<TYPE, CLASS, &CLASS::GETTER>
//#define WOPROP(TYPE, CLASS, SETTER) WOProperty<TYPE, CLASS, &CLASS::SETTER>
//#define RWPROP(TYPE, CLASS, GETTER, SETTER) RWProperty<TYPE, CLASS, &CLASS::GETTER, &CLASS::SETTER>

#endif //FOXENGINE_BASE_HPP
