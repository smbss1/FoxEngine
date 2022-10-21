
#ifndef FOXENGINE_ENTITY_SERIALIZER
#define FOXENGINE_ENTITY_SERIALIZER

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>

namespace fox
{
    class EntitySerializer
    {
    public:
        EntitySerializer();
        ~EntitySerializer();
	    static void SerializeAllEntityComponents(YAML::Emitter& out, Entity entity);
	    static void DeserializeAllEntryComponents(YAML::detail::iterator_value& entity, Entity& deserializedEntity);
    
    private:
        
    };
    
} // namespace fox


#endif