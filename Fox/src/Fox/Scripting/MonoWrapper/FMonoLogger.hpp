#include <functional>
#include <string>

namespace fox::mono
{
    using log_handler = std::function<void(const std::string&)>;
    void set_log_handler(const std::string& category, const log_handler& handler);
    auto get_log_handler(const std::string& category) -> const log_handler&;
} // namespace fox::mono