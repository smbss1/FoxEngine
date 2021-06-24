//
// Created by samuel on 23/06/2021.
//

#ifndef FOXENGINE_SERIALIZATION_HPP
#define FOXENGINE_SERIALIZATION_HPP

#include "json.hpp"
#include "Utils/Vec2.hpp"

namespace fox
{
    namespace json
    {
        template <>
        struct Serializer<Timeline>
        {
            static void serialize(Value& j, const Timeline& value)
            {
            }

            static void deserialize(const Value& j, Timeline& value)
            {
                value.loop(j["loop"].get<bool>());
                if (j["tracks"].is_array()) {
                    value.m_vTracks.reserve(j["tracks"].get<Array>().size());
                    for (auto &elem : j["tracks"].get<Array>()) {
                        if (elem["type"].get<std::string>() == "float")
                            value.m_vTracks.push_back(elem.get<std::unique_ptr<Track<float>>>());
                        else if (elem["type"].get<std::string>() == "Vec3")
                            value.m_vTracks.push_back(elem.get<std::unique_ptr<Track<Vec3>>>());
                        else if (elem["type"].get<std::string>() == "Quat")
                            value.m_vTracks.push_back(elem.get<std::unique_ptr<Track<quat>>>());
//                        else if (elem["type"].get<std::string>() == "Vec2")
//                            value.m_vTracks.push_back(elem.get<std::unique_ptr<Track<Vec2>>>());
                    }
                }
            }
        };

        template <typename T>
        struct Serializer<std::unique_ptr<T>>
        {
            static void serialize(Value& j, const std::unique_ptr<T>& value)
            {
            }

            static void deserialize(const Value& j, std::unique_ptr<T>& value)
            {
                value = std::make_unique<T>(j.template get<T>());
            }
        };

        template <typename T>
        struct Serializer<Track<T>>
        {
            static void serialize(Value& j, const Track<T>& value)
            {
            }

            static void deserialize(const Value& j, Track<T>& value)
            {
                std::vector<Frame<T>> m_vFrames = j["keys"].get<std::vector<Frame<T>>>();
                for (auto& frame : m_vFrames)
                    value.then(frame.m_fTime, frame.m_In, frame.m_Out, frame.m_Value);
            }
        };

        template <typename T>
        struct Serializer<Frame<T>>
        {
            static void serialize(Value& j, const Frame<T>& value)
            {
            }

            static void deserialize(const Value& j, Frame<T>& value)
            {
                value.m_fTime = j["time"].template get<float>();
                value.m_Value = j["value"].template get<T>();
                value.m_In = j["in"].template get<T>();
                value.m_Out = j["out"].template get<T>();
            }
        };

        template <>
        struct Serializer<Vec3>
        {
            static void serialize(Value& j, const Vec3& value)
            {
                j["x"] = value.x;
                j["y"] = value.y;
                j["z"] = value.z;
            }

            static void deserialize(const Value& j, Vec3& value)
            {
                value.x = j["x"].template get<float>();
                value.y = j["y"].template get<float>();
                value.z = j["z"].template get<float>();
            }
        };

        template <>
        struct Serializer<quat>
        {
            static void serialize(Value& j, const quat& value)
            {
                j["x"] = value.x;
                j["y"] = value.y;
                j["z"] = value.z;
                j["w"] = value.w;
            }

            static void deserialize(const Value& j, quat& value)
            {
                value.x = j["x"].template get<float>();
                value.y = j["y"].template get<float>();
                value.z = j["z"].template get<float>();
                value.w = j["w"].template get<float>();
            }
        };

        template <>
        struct Serializer<Vec2>
        {
            static void serialize(Value& j, const Vec2& value)
            {
                j["x"] = value.x;
                j["y"] = value.y;
            }

            static void deserialize(const Value& j, Vec2& value)
            {
                value.x = j["x"].template get<float>();
                value.y = j["y"].template get<float>();
            }
        };
    }
}

#endif //FOXENGINE_SERIALIZATION_HPP
