
#ifndef FOXENGINE_ENTITY_SERIALIZER
#define FOXENGINE_ENTITY_SERIALIZER

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>

namespace fox
{
    class EntitySerializer
    {
    public:
	    static void SerializeAllEntityComponents(YAML::Emitter& out, Entity entity);
	    static void DeserializeAllEntryComponents(YAML::Node& entity, Entity& deserializedEntity);
        static void SerializeEntityAsPrefab(const char* filepath, Entity entity);
        static Entity DeserializeEntityAsPrefab(const fs::path& filepath, Scene& scene);

    private:

    };

} // namespace fox


#endif
