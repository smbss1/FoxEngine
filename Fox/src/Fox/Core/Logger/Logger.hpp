//
// Created by samuel on 14/06/2021.
//

#ifndef TCPSERVER_LOGGER_HPP
#define TCPSERVER_LOGGER_HPP

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

    enum typelog {
        DEBUG,
        INFO,
        WARN,
        ERROR
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
        std::string get_type_str(typelog type)
        {
            std::string label;
            switch(type) {
                case DEBUG: label = "DEBUG"; break;
                case INFO:  label = "INFO "; break;
                case WARN:  label = "WARN "; break;
                case ERROR: label = "ERROR"; break;
            }
            return label;
        }

        /**
         * Display a log message in a given output
         * @param[in] type	Type to display
         * @param[in] msg	Message to display
         * @param[in] args  the list of arguments to display
         */
        template <typename... Args,
                std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
        void log(typelog type, const std::string& msg, const Args&... args)
        {
            // Lock the output
            std::lock_guard<std::mutex> locker(mutex);
            // Put and flush the message
            *output_stream << "[" << get_type_str(type) << "] ";
            *output_stream << print(msg, args...) << std::endl;
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
    void log(typelog type, const std::string& msg, const Args&... args)
    {
        Logger::instance().log(type, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void info(const std::string& msg, const Args&... args)
    {
        log(INFO, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void warn(const std::string& msg, const Args&... args)
    {
        log(WARN, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void error(const std::string& msg, const Args&... args)
    {
        log(ERROR, msg, args...);
    }

    template <typename... Args,
            std::enable_if_t<std::conjunction_v<is_ostreamable<Args>...>, int> = 0>
    void debug(const std::string& msg, const Args&... args)
    {
        log(DEBUG, msg, args...);
    }
}

#endif //TCPSERVER_LOGGER_HPP
