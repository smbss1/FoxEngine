//
// Created by samuel on 30/06/2021.
//

#ifndef FOXENGINE_PLATFORMUTILS_HPP
#define FOXENGINE_PLATFORMUTILS_HPP

#include <string>
#include <vector>

namespace fox {

    class FileDialogs
    {
    public:
        // These return empty strings if cancelled
        static std::string OpenFile(const std::vector<std::string>& filter);
        static std::string SaveFile(const std::vector<std::string>& filter);
    };

    class Time
    {
    public:
        static float GetTime();

        static float time;
        static float delta_time;
        static float fixed_delta_time;
        static float factor_physics;
    };
}


#endif //FOXENGINE_PLATFORMUTILS_HPP
