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


#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L && defined(__has_include)
    // ^ Supports MSVC prior to 15.7 without setting /Zc:__cplusplus to fix __cplusplus
    // _MSVC_LANG works regardless. But without the switch, the compiler always reported 199711L: https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/
    #if __has_include(<filesystem>) // Two stage __has_include needed for MSVC 2015 and per https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005finclude.html
        #define GHC_USE_STD_FS

        // Old Apple OSs don't support fs, though the header is available at compile
        // time. In particular, fs is unavailable before macOS 10.15, iOS/tvOS 13.0,
        // and watchOS 6.0.
        #ifdef __APPLE__
            #include <Availability.h>
            // Note: This intentionally uses fs on any new Apple OS, like visionOS
            // released after fs, where fs is always available.
            // (All other __<platform>_VERSION_MIN_REQUIREDs will be undefined and thus 0.)
            #if __MAC_OS_X_VERSION_MIN_REQUIRED && __MAC_OS_X_VERSION_MIN_REQUIRED < 101500 \
             || __IPHONE_OS_VERSION_MIN_REQUIRED && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000 \
             || __TV_OS_VERSION_MIN_REQUIRED && __TV_OS_VERSION_MIN_REQUIRED < 130000 \
             || __WATCH_OS_VERSION_MAX_ALLOWED && __WATCH_OS_VERSION_MAX_ALLOWED < 60000
                #undef GHC_USE_STD_FS
            #endif  
        #endif
    #endif
#endif

#ifdef GHC_USE_STD_FS
    #include <filesystem>
    namespace fs = std::filesystem;
#else
    #include "filesystem.hpp"
    namespace fs = ghc::filesystem;
#endif

    template <typename T>
    struct reversion_wrapper { T& iterable; };

    template <typename T>
    auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

    template <typename T>
    auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

    template <typename T>
    reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

namespace fox
{
    void InitializeCore();
    void ShutdownCore();
}


#endif //FOXENGINE_BASE_HPP
