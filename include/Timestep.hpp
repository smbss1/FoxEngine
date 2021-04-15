

#ifndef FOX_TIMESTEP_HPP_
#define FOX_TIMESTEP_HPP_

class Timestep
{
public:
    Timestep(float fTime = 0.0f) : m_fTime(fTime)
    {

    }

    ~Timestep()
    {

    }

    float get_millisecond() const
    {
        return m_fTime * 1000.f;
    }

    explicit operator float() const
    {
        return m_fTime;
    }
    
private:
    float m_fTime;
};


#endif