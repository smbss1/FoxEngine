/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Timeline.hpp
*/

#ifndef FOX_TIMELINE_HPP_
#define FOX_TIMELINE_HPP_

#include "Time.hpp"
#include <Animator/Easing.hpp>
#include <Animator/Track.hpp>
#include <unordered_map>

class Timeline
{
public:
    /**
     * @brief Constructor
     */
    explicit Timeline() : m_bLoop(false), m_fTime(0) {}

    /**
     * @brief Constructor
     * @param loop set to true if the timeline need to loop
     */
    explicit Timeline(bool loop) : m_bLoop(loop), m_fTime(0) {}

    /**
     * @brief Destructor
     */
    ~Timeline() = default;

    /**
     * @brief Copy Constructor
     */
    Timeline(Timeline&& other)
            : m_Signals(std::move(other.m_Signals))
            , m_vTracks(std::move(other.m_vTracks))
            , m_bLoop(std::move(other.m_bLoop))
            , m_fTime(std::move(other.m_fTime))
    {
    }

    Timeline& operator=(Timeline&& rhs)
    {
        m_Signals = std::move(rhs.m_Signals);
        m_vTracks = std::move(rhs.m_vTracks);
        m_bLoop = std::move(rhs.m_bLoop);
        m_fTime = std::move(rhs.m_fTime);
        return *this;
    }

    /**
     * @brief Add a new track
     * @tparam T The type of the value to modified during the timeline execution
     * @param e the type of interpolation to use
     * @return the new created Track class
     */
    template<typename T>
    Track<T>& add(Easing e)
    {
        Track<T>* result = new Track<T>();
        result->ease(e);
        m_vTracks.push_back(std::unique_ptr<ITrack>(result));
        for (auto &track : m_vTracks) {
            if (track->IsValid())
                m_fEndTime = std::max(m_fEndTime, track->GetEndTime());
        }
        return *result;
    }

    /**
     * @brief Add a new track
     * @tparam T The type of the value to modified during the timeline execution
     * @return the new created Track class
     */
    template<typename T>
    Track<T>& add()
    {
        Track<T>* result = new Track<T>();
        m_vTracks.push_back(std::unique_ptr<ITrack>(result));

        for (auto &track : m_vTracks) {
            if (track->IsValid())
                m_fEndTime = std::max(m_fEndTime, track->GetEndTime());
        }
        return *result;
    }

    /**
     * @brief Allow the timeline to loop or not
     * @param v set to true if the timeline need to loop
     * @return the class itself
     */
    Timeline& loop(bool v)
    {
        m_bLoop = v;
        return *this;
    }

    /**
     * @brief Add a callback function to be called when the time execution equal
     * to the time given in parameter
     * @param time the time of when the signal need to be trigger
     * @param cb the signal function callback
     * @return the class itself
     */
    Timeline& signal(float time, std::function<void()> cb)
    {
        m_Signals.emplace(time, cb);
        return *this;
    }

    /**
     * @brief Reset the time of the timeline
     */
    void reset()
    {
        m_fSignalTime = 0;
    }

    /**
     * @brief Run the animation
     */
    void run()
    {
        m_fTime += Time::delta_time;
        m_fSignalTime += Time::delta_time;

//        if (m_fSignalTime >= m_fEndTime && m_bLoop)
//            m_fSignalTime = 0;

        for (auto &track : m_vTracks) {
            if (track->IsValid())
                track->Sample(m_fTime, m_bLoop);
        }

        for (auto &sig : m_Signals) {
            if (are_same(sig.first, m_fSignalTime)) {
                sig.second();
            }
        }
    }

private:
    bool are_same(float a, float b)
    {
        return fabs(a - b) < 0.05f;
    }

public:
    std::unordered_map<float, std::function<void()>> m_Signals;
    std::vector<std::unique_ptr<ITrack>> m_vTracks;
    bool m_bLoop;
    float m_fTime = 0;
    float m_fSignalTime = 0;
    float m_fEndTime = 0;
};

#endif