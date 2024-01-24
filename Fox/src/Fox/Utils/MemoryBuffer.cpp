#include "MemoryBuffer.hpp"

namespace fox
{
    template<>
    void MemoryBuffer::write(const glm::vec4& value)
    {
        write(&value, sizeof(glm::vec4));
    }

    template<>
    void MemoryBuffer::write(const glm::mat4& value)
    {
        write(&value[0], sizeof(glm::mat4));
    }
}