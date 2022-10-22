//
// Created by samuel on 14/06/2021.
//

#ifndef TCPSERVER_LOGGER_HPP
#define TCPSERVER_LOGGER_HPP

#include "Events/EventSystem.hpp"

#include <mutex>
#include <string>
#include <sstream>
#include <thread>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include <cassert>
#include <iostream>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace fox
{
    namespace detail
    {
        template <typename It>
        void print(std::ostream& os, It first, It last)
        {
            for (auto it = first; it != last; ++it) {
                switch (*it) {
                    case '%':
                        throw std::invalid_argument{"too few arguments"};
                    case '/':
                        ++it;
                        if (it == last)
                            throw std::invalid_argument{"stray '/'"};
                        [[fallthrough]];
                    default:
                        os << *it;
                }
            }
        }

        template <typename It, typename T, typename... Args>
        void print(std::ostream& os, It first, It last,
                   const T& arg, const Args&... args)
        {
            for (auto it = first; it != last; ++it) {
                switch (*it) {
                    case '%':
                        os << arg;
                        return print(os, ++it, last, args...);
                    case '/':
                        ++it;
                        if (it == last)
                            throw std::invalid_argument{"stray '/'"};
                        [[fallthrough]];
                    default:
                        os << *it;
                }
            }
            throw std::invalid_argument{"too many arguments"};
        }

        template <typename... Args>
        struct Printer {
            std::string_view format;
            std::tuple<const Args&...> args;
        };

        template <typename... Args, std::size_t... Is>
        void printer_helper(std::ostream& os, const Printer<Args...>& printer,
                            std::index_sequence<Is...>)
        {
            print(os, printer.format.begin(), printer.format.end(),
                  std::get<Is>(printer.args)...);
        }

        template <typename... Args>
        std::ostream& operator<<(std::ostream& os, const Printer<Args...>& printer)
        {
            printer_helper(os, printer, std::index_sequence_for<Args...>{});
            return os;
        }
    }

    template <typename T, typename = void>
    struct is_ostreamable :std::false_type {};
    template <typename T>
    struct is_ostreamable<T, std::void_t<
            decltype(std::declval<std::ostream>() << std::declval<T>())>
    > :std::true_type {};
    template <typename T>
    inline constexpr bool is_ostreamable_v = is_ostreamable<T>::value;

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    auto print(std::string_view format, const Args&... args)
    {
        return detail::Printer<Args...>{format, std::forward_as_tuple(args...)};
    }

    template <typename... Args,
        std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    std::string format(std::string_view format, const Args&... args)
    {
        std::stringstream str;
        str << detail::Printer<Args...>{format, std::forward_as_tuple(args...)};
        return str.str();
    }

    enum typelog
    {
        INFO = (1 << 0),
        WARN = (1 << 1),
        ERROR = (1 << 2)
    };

    struct OnConsoleLogEvent
    {
        OnConsoleLogEvent(std::string message, typelog log) : Message(message), Log(log)
        {
        }
        std::string Message;
        typelog Log;
    };

    /**
     * This class implements a logging mechanism. Messages are shown sequentially on the screen
     */
    class Logger
    {
    private:
        /**
         * Mutex used to lock the output
         */
        std::mutex mutex;

        /**
         * Class constructor
         */
        Logger() : output_stream(&std::cout)
        {
        };

        template<typename T>
        std::string to_str(T t) {
            std::stringstream os;
            os << t;
            return os.str();
        };

    public:

        static Logger& instance()
        {
            static Logger logger;
            return logger;
        }

        /**
         * Get the Type enum to string
         * @param[in] type		The enum type
         * @return				String name of the enum
         */
        static const char* get_type_str(typelog type)
        {
            switch(type) {
                case INFO:  return "INFO ";
                case WARN:  return "WARN ";
                case ERROR: return "ERROR";
            }
            return "Unknown name";
        }

        /**
         * Display a log message in a given output
         * @param[in] type	Type to display
         * @param[in] msg	Message to display
         * @param[in] args  the list of arguments to display
         */
        template <typename... Args,
                std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
        void log(typelog type, std::string_view tag, const std::string& msg, const Args&... args)
        {
            // Lock the output
            std::lock_guard<std::mutex> locker(mutex);
            std::stringstream stream;

            // Put and flush the message
            stream << print(msg, args...);
            *output_stream << "[" << get_type_str(type) << "] ";
            if (!tag.empty())
            {
                *output_stream << "[" << tag << "] ";
            }
            *output_stream << stream.str() << std::endl;

            event::EventSystem::Get().template Emit(OnConsoleLogEvent(stream.str(), type));
        }

        void set_output_stream(std::ostream& stream)
        {
            output_stream = &stream;
        }

        std::ostream* output_stream;
    };

    /**
     * Convert a date/time into a string
     * @param[in] time	Date/time to convert
     * @return			Converted date/time (in string format) with the time difference (if defined)
     */
    std::string dateToString(const std::chrono::system_clock::time_point& time);
    std::string dateToString(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end);

    /**
     * Calculate the diff time between two time points
     * with the difference (in milliseconds)
     * @param[in] start		Start time
     * @param[in] end		Finished time
     * @return				Difference of end-start
     */
    int diffTimeToString(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end);

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void log(typelog type, std::string_view tag, const std::string& msg, const Args&... args)
    {
        Logger::instance().log(type, tag, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void info(std::string_view tag, const std::string& msg, const Args&... args)
    {
        log(INFO, tag, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void warn(std::string_view tag, const std::string& msg, const Args&... args)
    {
        log(WARN, tag, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void error(std::string_view tag, const std::string& msg, const Args&... args)
    {
        log(ERROR, tag, msg, args...);
    }
}

// Core log macros
#define FOX_CORE_INFO(...)     ::fox::info("", __VA_ARGS__)
#define FOX_CORE_WARN(...)     ::fox::warn("", __VA_ARGS__)
#define FOX_CORE_ERROR(...)    ::fox::error("", __VA_ARGS__)

// Client log macros
#define FOX_INFO(...)          ::fox::info("", __VA_ARGS__)
#define FOX_WARN(...)          ::fox::warn("", __VA_ARGS__)
#define FOX_ERROR(...)         ::fox::error("", __VA_ARGS__)

// Core log macros
#define FOX_CORE_INFO_TAG(tag, ...)     ::fox::info(tag, __VA_ARGS__)
#define FOX_CORE_WARN_TAG(tag, ...)     ::fox::warn(tag, __VA_ARGS__)
#define FOX_CORE_ERROR_TAG(tag, ...)    ::fox::error(tag, __VA_ARGS__)

// Client log macros
#define FOX_INFO_TAG(tag, ...)          ::fox::info(tag, __VA_ARGS__)
#define FOX_WARN_TAG(tag, ...)          ::fox::warn(tag, __VA_ARGS__)
#define FOX_ERROR_TAG(tag, ...)         ::fox::error(tag, __VA_ARGS__)

#endif //TCPSERVER_LOGGER_HPP
