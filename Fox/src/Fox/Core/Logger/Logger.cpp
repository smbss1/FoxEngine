#include "Logger.hpp"
#include "Core/Logger/Logger.hpp"
#include <memory>

namespace fox
{
    std::shared_ptr<Logger> Log::s_CoreLogger;
    std::shared_ptr<Logger> Log::s_ClientLogger;
    std::shared_ptr<Logger> Log::s_EditorConsoleLogger;

    void Log::Init()
    {
        s_CoreLogger = std::make_shared<Logger>();
        s_ClientLogger = std::make_shared<Logger>();
        s_EditorConsoleLogger = std::make_shared<Logger>();
    }

    void Log::Shutdown()
    {
        s_CoreLogger.reset();
        s_ClientLogger.reset();
        s_EditorConsoleLogger.reset();
    }
}