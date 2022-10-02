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
#include "Core/Base.hpp"
#include "Core/Property.hpp"

struct Signal
{
    std::function<void()> m_func;
    bool m_bCalled = false;
};

class Timeline
{
    using TrackArray = std::vector<fox::scope<ITrack>>;
public:
    /**
     * @brief Constructor
     */
    explicit Timeline()
    {
    }

    explicit Timeline(const std::string& strName) : Timeline()
    {
        Name = strName;
    }

    /**
     * @brief Constructor
     * @param loop set to true if the timeline need to loop
     */
    explicit Timeline(bool loop) : Timeline()
    {
        m_bLoop = loop;
    }

    /**
     * @brief Destructor
     */
    ~Timeline() = default;

    /**
     * @brief Copy Constructor
     */
    Timeline(Timeline&& other)
            : Signals(std::move(other.Signals))
            , Tracks(std::move(other.Tracks))
            , m_bLoop(std::move(other.m_bLoop))
            , m_fTime(std::move(other.m_fTime))
            , Name(std::move(other.Name))
            , ID(std::move(other.ID))
    {
    }

    Timeline& operator=(Timeline&& rhs)
    {
        Signals = std::move(rhs.Signals);
        Tracks = std::move(rhs.Tracks);
        m_bLoop = std::move(rhs.m_bLoop);
        m_fTime = std::move(rhs.m_fTime);
        Name = std::move(rhs.Name);
        ID = std::move(rhs.ID);
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
        Tracks.get().push_back(std::unique_ptr<ITrack>(result));
        OnTrackAdded(&Tracks);
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
        Tracks.get().push_back(std::unique_ptr<ITrack>(result));
        OnTrackAdded(&Tracks);
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
        return *static_cast<Track<T>*>(Tracks[idx].get());
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
        Signals.get().emplace(time, Signal{cb, false});
        return *this;
    }

    /**
     * @brief Reset the time of the timeline
     */
    void reset()
    {
        IsFinish = false;
        m_fSignalTime = 0;
        m_fTime = 0;

        for (auto &sig : Signals.get()) {
            sig.second.m_bCalled = false;
        }
    }

    void OnTrackAdded(property<TrackArray>* tracks)
    {
        for (auto &track : tracks->get()) {
            m_fEndTime = std::max(m_fEndTime, track->GetEndTime());
        }
        if (m_fEndTime <= 0)
            m_fEndTime = 5.0f;
    }

    /**
     * @brief Run the animation
     */
    void run()
    {
        if (m_fSignalTime <= m_fEndTime) {
            m_fSignalTime += Time::delta_time;
            m_fTime += Time::delta_time;

            for (auto &track : Tracks.get()) {
                if (track->IsValid()) {
                    track->Sample(m_fTime, m_bLoop);
                }
            }

            for (auto &sig : Signals.get()) {
                if (!sig.second.m_bCalled && sig.first <= m_fSignalTime) {
                    sig.second.m_func();
                    sig.second.m_bCalled = true;
                }
            }
        } else {
            IsFinish = true;
            if (m_bLoop) {
                reset();
            }
        }
    }

private:

public:
    property<std::string> Name;
    property<int> ID;
    property<TrackArray> Tracks;
    property<std::unordered_map<float, Signal>> Signals;
    property<bool> IsFinish = false; // Is Finish the animation ?

private:
    bool m_bLoop = false;
    float m_fTime = 0;
    float m_fSignalTime = 0;
    float m_fEndTime = 2;
};

#endif