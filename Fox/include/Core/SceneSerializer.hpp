//
// Created by samuel on 30/06/2021.
//

#ifndef FOXENGINE_SCENESERIALIZER_HPP
#define FOXENGINE_SCENESERIALIZER_HPP

#include "Scene.hpp"

namespace fox
{
    class SceneSerializer
    {
    public:
        explicit SceneSerializer(const ref<Scene>& scene);

        void Serialize(const std::string& filepath);
        void SerializeRuntime(const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

    private:
        ref<Scene> m_pScene;
    };
}

#endif //FOXENGINE_SCENESERIALIZER_HPP
