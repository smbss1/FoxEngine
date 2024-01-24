
#include "Base.hpp"
#include "Logger/Logger.hpp"

namespace fox
{
    void InitializeCore()
    {
        Log::Init();
    }

    void ShutdownCore()
    {
        Log::Shutdown();
    }
}