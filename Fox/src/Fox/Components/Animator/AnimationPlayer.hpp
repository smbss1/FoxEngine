////
//// Created by samuel on 12/07/2021.
////
//
//#ifndef FOXENGINE_ANIMATIONPLAYER_HPP
//#define FOXENGINE_ANIMATIONPLAYER_HPP
//
//#include <unordered_map>
//#include <Utils/FileSystem.hpp>
//// #include <rttr/type>
//
//#include "Reflection.hpp"
//#include "Timeline.hpp"
//#include "Serialization.hpp"
//
//namespace fox
//{
//    class AnimationPlayer : public ComponentImpl<AnimationPlayer>
//    {
//        // REFLECTABLEV(AnimationPlayer, Component)
//
//    public:
//        explicit AnimationPlayer() : current(nullptr), m_vAnimations()
//        {
//        }
//
//        ~AnimationPlayer() = default;
//
//        AnimationPlayer(AnimationPlayer &&other)
//                : m_vAnimations(std::move(other.m_vAnimations)), current(other.current)
//        {
//        }
//
//        AnimationPlayer &operator=(AnimationPlayer &&rhs)
//        {
//            m_vAnimations = std::move(rhs.m_vAnimations);
//            current = rhs.current;
//            return *this;
//        }
//
//        Timeline &AddAnimation(const std::string &name)
//        {
//            std::unique_ptr<Timeline> anim = std::make_unique<Timeline>();
//            Timeline *ptr = anim.get();
//            anim->Name = name;
//            m_vAnimations.push_back(std::move(anim));
//            if (!current)
//                current = ptr;
//            return *ptr;
//        }
//
//        Timeline &add_anim_from_file(const std::string &strFilepath)
//        {
//            std::string out;
//            if (fox::file::ReadFile(strFilepath, out))
//            {
//                fox::info("Parse animation file: %", strFilepath);
//
//                fox::json::Value anim_json = fox::json::parse(out);
//
//                std::string strAnimName = anim_json["name"].get<std::string>();
//                auto anim = anim_json.get<scope<Timeline>>();
//
//                fox::info("Animation Name: %", strAnimName);
//                fox::info("Animation Tracks count: %", anim->Tracks.get().size());
//                anim->Name = strAnimName;
//                Timeline *ptr = anim.get();
//                m_vAnimations.push_back(std::move(anim));
//                if (!current)
//                    current = ptr;
//                return *ptr;
//            } else
//                throw std::runtime_error("Cannot find the animation file: " + strFilepath);
//        }
//
//        void play(const std::string &strName)
//        {
//            auto it = FindAnimation(strName);
//            if (it)
//            {
//                if (current)
//                    current->reset();
//                current = it;
//            }
//        }
//
//        void stop()
//        {
//            current = nullptr;
//        }
//
//        void run()
//        {
//            if (current)
//                current->run();
//        }
//
//    private:
//        Timeline* FindAnimation(const std::string &strName)
//        {
//            auto it = m_vAnimations.begin();
//            for (; it != m_vAnimations.end(); ++it)
//            {
//                if (it->get()->Name == strName)
//                    return it->get();
//            }
//        }
//
//    public:
//        rw_property<Timeline*> Current {
//            GET { return current; },
//            SET { current = value; }
//        };
//
//    private:
//        std::vector<scope<Timeline>> m_vAnimations;
//        Timeline* current;
//    };
//}
//
//#endif //FOXENGINE_ANIMATIONPLAYER_HPP
