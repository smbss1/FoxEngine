
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include "FoxEcs.hpp"

namespace fox
{
    class Application
    {
    public:
        Application();
        ~Application();

        void run();

    private:
        bool m_bIsRunning;

        World oWorld;
    };
}

#endif