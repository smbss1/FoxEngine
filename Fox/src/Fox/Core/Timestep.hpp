//
// Created by samuel on 05/10/22.
//

#ifndef FOXENGINE_TIMESTEP_HPP
#define FOXENGINE_TIMESTEP_HPP

namespace fox
{
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : m_Time(time)
        {
        }

        operator float() const { return m_Time; }

        float GetSeconds() const { return m_Time; }
        float GetMilliseconds() const { return m_Time * 1000.0f; }
    private:
        float m_Time;
    };
}

#endif //FOX_LEXER_TIMESTEP_HPP
