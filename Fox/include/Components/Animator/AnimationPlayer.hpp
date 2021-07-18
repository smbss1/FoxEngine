//
// Created by samuel on 12/07/2021.
//

#ifndef FOXENGINE_ANIMATIONPLAYER_HPP
#define FOXENGINE_ANIMATIONPLAYER_HPP

#include <unordered_map>
#include <FileSystem/FileSystem.hpp>
#include <rttr/type>

#include "Reflection.hpp"
#include "Timeline.hpp"
#include "Serialization.hpp"
#include "Components/Component.hpp"

namespace fox
{
    class AnimationPlayer : public ComponentImpl<AnimationPlayer>
    {
        REFLECTABLEV(AnimationPlayer, Component)

        using AnimationPtr = scope<Timeline>;
        using AnimationList = std::vector<AnimationPtr>;
        using AnimationIter = AnimationList::iterator;

    public:
        explicit AnimationPlayer() : m_vAnimations()
        {
            Current = nullptr;
        }

        ~AnimationPlayer() = default;

        AnimationPlayer(AnimationPlayer &&other)
                : m_vAnimations(std::move(other.m_vAnimations))
        {
            Current = other.current;
        }

        AnimationPlayer &operator = (AnimationPlayer &&rhs)
        {
            m_vAnimations = std::move(rhs.m_vAnimations);
            Current = rhs.current;
            return *this;
        }

        Timeline &AddAnimation(const std::string_view &name)
        {
            std::unique_ptr<Timeline> anim = std::make_unique<Timeline>();
            anim->Name = name;
            Timeline *ptr = anim.get();
            m_vAnimations.push_back(std::move(anim));
            if (!current)
                Current = ptr;
            return *ptr;
        }

        void RemoveAnimation(const std::string_view &name)
        {
            m_vAnimations.erase(FindAnimation(name));
        }

        Timeline* GetAnimation(const std::string_view &name)
        {
            return FindAnimation(name)->get();
        }

        Timeline &add_anim_from_file(const std::string &strFilepath)
        {
            std::string out;
            if (fox::file::ReadFile(strFilepath, out))
            {
                fox::info("Parse animation file: %", strFilepath);

                fox::json::Value anim_json = fox::json::parse(out);

                std::string strAnimName = anim_json["name"].get<std::string>();
                auto anim = anim_json.get<scope<Timeline>>();

                fox::info("Animation Name: %", strAnimName);
                fox::info("Animation Tracks count: %", anim->Tracks.get().size());
                anim->Name = strAnimName;
                Timeline *ptr = anim.get();
                m_vAnimations.push_back(std::move(anim));
                if (!current)
                    Current = ptr;
                return *ptr;
            } else
                throw std::runtime_error("Cannot find the animation file: " + strFilepath);
        }

        void play(const std::string &strName)
        {
            auto it = FindAnimation(strName);
            if (it != m_vAnimations.end())
            {
                if (current)
                    current->reset();
                Current = it->get();
            }
        }

        void stop()
        {
            Current = nullptr;
        }

        void run()
        {
            if (current)
                current->run();
        }

    private:
        AnimationIter FindAnimation(const std::string_view& strName)
        {
            return std::find_if(m_vAnimations.begin(), m_vAnimations.end(), [&](AnimationPtr& it) {
                return it->Name.get() == strName;
            });
        }

    public:
        fox::properties::rw_property<Timeline*> Current {
            GET { return current; },
            SET { current = value; }
        };

        fox::properties::read_property<std::vector<scope<Timeline>>> Animations {
                GET -> const AnimationList& { return m_vAnimations; }
        };
    private:
        const AnimationList& GetAnimations()
        {
            return m_vAnimations;
        }

    private:
        AnimationList m_vAnimations;
        Timeline* current;
    };
}

#endif //FOXENGINE_ANIMATIONPLAYER_HPP
