/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** AudioManager.cpp
*/

#include "Save for later/AudioManager.hpp"

namespace fox {
    AudioManager::AudioManager() {}

    void AudioManager::CreateGroup(const std::string &name)
    {
        groups.push_back(AudioGroup(name));
    }

    AudioGroup &AudioManager::getGroup(const std::string &name)
    {
        for (auto it = groups.begin(); it != groups.end(); it++) {
            if (it->getAudioName() == name)
                return *it;
        }
        throw std::runtime_error("AudioGroup: name not found");
    }

    void AudioManager::PlaySound(const std::string &group, const std::string &asset, float volume, float pitch)
    {
//        auto it = m_vSounds.find(asset);
//        if (it == m_vSounds.end())
//            m_vSounds[asset] = std::make_unique<ray::Sound>(asset);
//        AudioGroup& g = getGroup(group);
//        auto sound = m_vSounds[asset].get();
//        sound->SetVolume(volume * g.getVolume());
//        sound->SetPitch(pitch);
//        sound->Play();
    }

    void AudioManager::PlayMusic(const std::string &group, const std::string &asset, float volume, float pitch)
    {
//        auto it = m_vMusics.find(asset);
//        if (it == m_vMusics.end())
//            m_vMusics[asset] = std::make_unique<ray::Music>(asset);
//        AudioGroup& g = getGroup(group);
//        auto sound = m_vMusics[asset].get();
//        sound->SetVolume(volume * g.getVolume());
//        SetMusicPitch(*sound, pitch);
//        sound->Play();
    }

    void AudioManager::StopSound(const std::string &asset)
    {
//        auto sound = m_vSounds[asset].get();
//        sound->Stop();
    }
}
