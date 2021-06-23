/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** AudioGroup.cpp
*/

#include "Core/AudioGroup.hpp"

namespace fox {
    AudioGroup::AudioGroup() : Volume(1) {}

    AudioGroup::AudioGroup(const std::string &name) : Volume(1)
    {
        AudioName = name;
    }

    std::string AudioGroup::getAudioName() const
    {
        return AudioName;
    }

    void AudioGroup::setAudioName(const std::string &name)
    {
        AudioName = name;
    }

    float AudioGroup::getVolume() const
    {
        return Volume;
    }

    void AudioGroup::setVolume(float value)
    {
        Volume = value;
    }
}