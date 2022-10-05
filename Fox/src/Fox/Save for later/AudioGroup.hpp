/*
** EPITECH PROJECT, 2021
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** AudioGroup.hpp header
*/


#ifndef AUDIO_GROUP_HPP_
#define AUDIO_GROUP_HPP_

#include <iostream>

namespace fox {
    class AudioGroup
    {
        public:
            /**
             * @brief Constructor of the class
             */
            AudioGroup();

            /**
             * @brief Constructor of the class
             * @param name the name of the audio group
             */
            AudioGroup(const std::string &name);

            /**
             * @brief Get the name of the group
             * @return the name of the group
             */
            std::string getAudioName() const;

            /**
             * @brief Set the name
             * @param name the new name
             */
            void setAudioName(const std::string &name);

            /**
             * @brief Get the volume of the group
             * @return the volume of the group
             */
            float getVolume() const;

            /**
             * @brief Set the volume
             * @param value the new volume value
             */
            void setVolume(float value);
        private:
            std::string AudioName;
            float Volume;
    };
}

#endif // AUDIO_GROUP_HPP_