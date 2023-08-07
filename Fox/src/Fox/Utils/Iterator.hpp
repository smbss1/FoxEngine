//
// Created by samuel on 05/11/22.
//

#ifndef FOX_LEXER_ITERATOR_HPP
#define FOX_LEXER_ITERATOR_HPP

#include <iterator>  // for iterator_traits, begin, end
#include <optional>
#include <utility>   // for forward, move
//
//template <typename Tin, typename Tout>
//struct TransformedRange;
//
//template <typename Range, typename Pred>
//struct FilteredRange;
//
//template <typename T>
//struct IITer : public std::enable_shared_from_this<IITer<T>>
//{
//    using value_type = T;
//    using Ptr = std::shared_ptr<IITer>;
//
//    virtual std::optional<value_type> next() = 0;
//
//    template <typename Tout>
//    auto transform(std::function<Tout(const T&)> function)
//    {
//        return std::make_shared<TransformedRange<T, Tout>>(this->shared_from_this(), function);
//    }
//
//    template <typename Pred>
//    auto filter(Pred&& pred)
//    {
//        return std::make_shared<FilteredRange<IITer<T>, Pred>>(this->shared_from_this(), std::forward<Pred>(pred));
//    }
//};
//
//// Non-owning input view based on STL iterators
//template <typename InputIt, typename Sentinel>
//struct InputRange : public IITer<typename std::iterator_traits<InputIt>::value_type>
//{
//    InputIt  current;
//    Sentinel end;
//
//    InputRange(InputIt it, Sentinel e)
//    {
//        current = it;
//        end = e;
//    }
//
//    using value_type = typename std::iterator_traits<InputIt>::value_type;
//
//    std::optional<value_type> next() override
//    {
//        if (current != end)
//            return *current++;
//        return std::nullopt;
//    }
//};
//
//// Range which filters elements of another range, based on a predicate
//template <typename Range, typename Pred>
//struct FilteredRange : Range
//{
//    using Ptr = typename Range::Ptr;
//    using value_type = typename Range::value_type;
//
//    Ptr range;
//    Pred pred;
//
//    FilteredRange(Ptr it, Pred&& e) : range(it), pred(e)
//    {
//    }
//
//    std::optional<value_type> next() override
//    {
//        while (const auto value = range->next())
//            if (pred(*value))
//                return value;
//        return std::nullopt;
//    }
//};
//
//// Range which applies a transform to another range
////template <typename Range, typename Func>
//template <typename Tin, typename Tout>
//struct TransformedRange : public IITer<Tout>
//{
//    using Ptr = typename IITer<Tin>::Ptr;
//    using value_type = Tout;
//
//    TransformedRange(Ptr r, std::function<Tout(const Tin&)> f) : range(r), func(f)
//    {
//    }
//
//    Ptr range;
//    std::function<Tout(const Tin&)> func;
//
//
//    std::optional<value_type> next() override
//    {
//        if (const auto value = range->next())
//            return func(*value);
//        return std::nullopt;
//    }
//};
//
//template <typename T>
//auto iter(T&& c) {
//    return std::make_shared<InputRange<decltype(std::begin(c)), decltype(std::end(c))>>(std::begin(c), std::end(c));
//}

// Non-owning input view based on STL iterators
template <typename InputIt, typename Sentinel>
struct InputRange {
    InputIt  current;
    Sentinel end;

    using value_type = typename std::iterator_traits<InputIt>::value_type;

    std::optional<value_type> next() {
        if (current != end)
            return *current++;
        else
            return std::nullopt;
    }
};

template <typename InputIt, typename Sentinel>
InputRange(InputIt, Sentinel) -> InputRange<InputIt, Sentinel>;

// Factory function taking anything with begin/end support and returning a mutable view
template <typename T>
auto iter(T&& c) {
    return InputRange{std::begin(c), std::end(c)};
}

// Range which filters elements of another range, based on a predicate
template <typename Range, typename Pred>
struct FilteredRange {
    Range range;
    Pred  pred;

    using value_type = typename Range::value_type;

    std::optional<value_type> next() {
        while (const auto value = range.next())
            if (pred(*value))
                return value;
        return std::nullopt;
    }
};

template <typename Range, typename Pred>
FilteredRange(Range, Pred) -> FilteredRange<Range, Pred>;

// Range which applies a transform to another range
template <typename Range, typename Func>
struct MapRange {
    Range range;
    Func  func;

    using value_type = decltype(func(*range.next()));

    std::optional<value_type> next() {
        if (const auto value = range.next())
            return func(*value);
        else
            return std::nullopt;
    }
};

template <typename Range, typename Func>
MapRange(Range, Func) -> MapRange<Range, Func>;

// Pipe-syntax enabler structs and operator overloads
template <typename Func>
struct MapProxy {
    Func func;
};

template <typename Func>
auto map(Func&& func) {
    return MapProxy<Func>{std::forward<Func>(func)};
}

template <typename Range, typename Func>
auto operator|(Range&& range, MapProxy<Func> proxy) {
    return MapRange{std::forward<Range>(range), std::move(proxy.func)};
}

template <typename Pred>
struct FilterProxy {
    Pred pred;
};

template <typename Pred>
auto filter(Pred&& pred) {
    return FilterProxy<Pred>{std::forward<Pred>(pred)};
}

template <typename Range, typename Pred>
auto operator|(Range&& range, FilterProxy<Pred> proxy) {
    return FilteredRange{std::forward<Range>(range), std::move(proxy.pred)};
}

#define LAM(dataType, data, call) [](dataType data) {call}

#endif //FOX_LEXER_ITERATOR_HPP
