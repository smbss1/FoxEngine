/* (c) 2018-2021 by dbj@dbj.org -- licence is at eof
        usage:
    auto demangled_name = dbj::demang< int (&) [42] >() ;
        This is both windows and linux code.
*/

#ifndef _WIN32
#include <cxxabi.h>
#endif
namespace dbj {
#ifdef _WIN32
    template <typename T>
    const char* demang() noexcept {
        static auto demangled_name = []() -> std::string { return { typeid(T).name() }; }();
        return demangled_name.data();
    }
#else   // __linux__
    template <typename T>
    const char* demang() noexcept {
        // once per one type
        static auto demangled_name = []() {
            int error = 0;
            std::string retval;
            char* name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &error);

            switch (error) {
            case 0:
                retval = name;
                break;
            case -1:
                retval = "memory allocation failed";
                break;
            case -2:
                retval = "not a valid mangled name";
                break;
            default:
                retval = "__cxa_demangle failed";
                break;
            }
            std::free(name);
            return retval;
            }();
            // std::free(name) ;
            return demangled_name.data();
    }
#endif  // __linux__
    // calling with instances
    template <typename T>
    const char* demang(T const&) noexcept {
        return demang<T>();
    }
}  // namespace dbj