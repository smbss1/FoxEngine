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
        explicit SceneSerializer(const Ref<Scene>& scene);

        bool Serialize(const fs::path& filepath);
        void SerializeRuntime(const fs::path& filepath);

        bool Deserialize(const fs::path& filepath);
        bool DeserializeRuntime(const fs::path& filepath);

    private:
        Ref<Scene> m_pScene;
    };
}

#endif //FOXENGINE_SCENESERIALIZER_HPP
