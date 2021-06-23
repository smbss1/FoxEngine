/*
** EPITECH PROJECT, 2021
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** AudioManager.hpp header
*/


#ifndef AUDIO_MANAGER_HPP_
#define AUDIO_MANAGER_HPP_

#include "Core/AudioGroup.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace fox {
    class AudioManager
    {
    public:
        /**
         * @brief Constructor of the class
         */
        AudioManager();

        /**
         * @brief Add a new @AudioGroup
         * @param name the name of the new audio group
         */
        void CreateGroup(const std::string &name);

        /**
         * @brief Get the audio group using the name
         * @param name the name to be search
         * @return a reference to the audio group or throw an exception if not found
         */
        AudioGroup &getGroup(const std::string &name);

        /**
         * @brief Play a sound
         * @param group the name of the group
         * @param asset the path to the sound file
         * @param volume the value of the volume between 0 and 1
         * @param pitch the value of the pitch between 0 and 1
         */
        void PlaySound(const std::string& group, const std::string& asset, float volume = 1, float pitch = 1);

        /**
         * @brief Play a music
         * @param group the name of the group
         * @param asset the path to the sound file
         * @param volume the value of the volume between 0 and 1
         * @param pitch the value of the pitch between 0 and 1
         */
        void PlayMusic(const std::string& group, const std::string& asset, float volume = 1, float pitch = 1);

        /**
         * @brief Stop a sound
         * @param asset the path to the sound file
         */
        void StopSound(const std::string &asset);

        private:
            std::vector<AudioGroup> groups;
//            std::unordered_map<std::string, std::unique_ptr<ray::Sound>> m_vSounds;
//            std::unordered_map<std::string, std::unique_ptr<ray::Music>> m_vMusics;
    };
}

#endif // AUDIO_MANAGER_HPP_