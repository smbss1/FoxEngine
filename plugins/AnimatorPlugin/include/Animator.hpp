/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Animator.hpp
*/

#ifndef FOX_ANIMATOR_HPP_
#define FOX_ANIMATOR_HPP_

#include <unordered_map>
#include <Utils/FileSystem.hpp>
#include "Timeline.hpp"
#include "Serialization.hpp"

class Animator
{
public:
    explicit Animator() : current(nullptr), m_vAnimations() {}
    ~Animator() = default;

    Animator(Animator&& other)
    : m_vAnimations(std::move(other.m_vAnimations)), current(other.current)
    {
    }

    Animator& operator=(Animator&& rhs)
    {
        m_vAnimations = std::move(rhs.m_vAnimations);
        current = rhs.current;
        return *this;
    }

    Timeline& add_anim(const std::string& name)
    {
        std::unique_ptr<Timeline> anim = std::make_unique<Timeline>();
        Timeline* ptr = anim.get();
        m_vAnimations.insert({name, std::move(anim)});
        if (!current)
            current = ptr;
        return *ptr;
    }

    void switch_to(const std::string& name)
    {
        auto it = m_vAnimations.find(name);
        if (it != m_vAnimations.end())
        {
            if (current)
                current->reset();
            current = it->second.get();
        }
    }

    Timeline& add_anim_from_file(const std::string& strFilepath)
    {
        std::string out;
        if (fox::file::ReadFile(strFilepath, out))
        {
            fox::info("Parse animation file: %", strFilepath);

            fox::json::Value anim_json = fox::json::parse(out);
            auto anim = anim_json.get<std::unique_ptr<Timeline>>();

            fox::info("Animation Name: %", anim_json["name"].get<std::string>());
            fox::info("Animation Tracks count: %", anim->m_vTracks.size());
            Timeline* ptr = anim.get();
            m_vAnimations.insert({anim_json["name"].get<std::string>(), std::move(anim)});
            if (!current)
                current = ptr;
            return *ptr;
        }
        else
            throw std::runtime_error("Cannot find the animation file: " + strFilepath);
    }

    void run()
    {
        if (current)
            current->run();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Timeline>> m_vAnimations;
    Timeline* current;
};

#endif