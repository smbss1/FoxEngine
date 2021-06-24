/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Timeline.hpp
*/

#ifndef FOX_TIMELINE_HPP_
#define FOX_TIMELINE_HPP_

#include <unordered_map>
#include "Time.hpp"
#include "Easing.hpp"
#include "Track.hpp"

struct Signal
{
    std::function<void()> m_func;
    bool m_bCalled = false;
};

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

        return *result;
    }

    /**
     * @brief Get a track at the index provide in parameter
     * @tparam T The type of the value to modified during the timeline execution
     * @param idx the index of the track
     * @return a reference to the track
     * @note this function throw a std::out_of_range if the index is incorrect
     */
    template<typename T>
    Track<T>& get(std::uint32_t idx)
    {
        return *static_cast<Track<T>*>(m_vTracks[idx].get());
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
        m_Signals.emplace(time, Signal{cb, false});
        return *this;
    }

    /**
     * @brief Reset the time of the timeline
     */
    void reset()
    {
        m_fSignalTime = 0;
        m_fTime = 0;

        for (auto &sig : m_Signals) {
            sig.second.m_bCalled = false;
        }
    }

    /**
     * @brief Run the animation
     */
    void run()
    {
        m_fTime += Time::delta_time;

        if (m_fSignalTime <= m_fEndTime) {
            m_fSignalTime += Time::delta_time;
        } else {
            if (m_bLoop && m_fSignalTime >= 0.01) {
                reset();
            }
        }

        for (auto &track : m_vTracks) {
            if (track->IsValid()) {
                track->Sample(m_fTime, m_bLoop);
                m_fEndTime = std::max(m_fEndTime, track->GetEndTime());
            }
        }

        for (auto &sig : m_Signals) {
            if (!sig.second.m_bCalled && sig.first <= m_fSignalTime) {
                sig.second.m_func();
                sig.second.m_bCalled = true;
            }
        }
    }

public:
    std::unordered_map<float, Signal> m_Signals;
    std::vector<std::unique_ptr<ITrack>> m_vTracks;
    bool m_bLoop;
    float m_fTime = 0;
    float m_fSignalTime = 0;
    float m_fEndTime = 0;
};

#endif