//
// Created by samuel on 15/06/2021.
//

#include "Core/Logger/Logger.hpp"

namespace fox
{
//    std::ostream& operator << (std::ostream& out, const std::chrono::system_clock::time_point &time)
//    {
//        return out << dateToString(time);
//    }

    std::string dateToString(const std::chrono::system_clock::time_point& time)
    {
        // Get time given the time point
        const std::time_t convTime = std::chrono::system_clock::to_time_t(time);
        // Get the local time given the converted time
        std::tm timetm = *std::localtime(&convTime);
        // Convert the time into a string
        std::stringstream oss;
        oss << std::put_time(&timetm, "%c %Z") ;
        return oss.str();
    }

    std::string dateToString(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end)
    {
        std::stringstream oss;
        oss << dateToString(start);
        oss << " - elapsed: " << diffTimeToString(start, end) << " ms";
        return oss.str();
    }

    int diffTimeToString(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end)
    {
        std::chrono::milliseconds milliStart = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch());
        std::chrono::milliseconds milliEnd   = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch());
        return milliEnd.count() - milliStart.count();
    }
}