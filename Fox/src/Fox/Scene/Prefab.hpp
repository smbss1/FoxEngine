//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_PREFAB_HPP
#define FOXENGINE_PREFAB_HPP

#include "Asset/Asset.hpp"

#include "Ecs/Entity.hpp"
#include "entt.hpp"

namespace fox
{
    class Prefab : public Asset
    {
    public:
        // Create prefab with empty entity
        Prefab();
        ~Prefab() override;

        // Replaces existing entity if present
        void Create(Entity entity, bool serialize = true);

        static AssetType GetStaticType() { return AssetType::Prefab; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }
    private:
        Entity CreatePrefabFromEntity(Entity entity);

        void OnScriptComponentConstruct(entt::registry& registry, entt::entity entity);
        void OnScriptComponentDestroy(entt::registry& registry, entt::entity entity);

    private:
        Ref<Scene> m_Scene;
        Entity m_Entity;

        friend class Scene;
        friend class PrefabEditor;
        friend class PrefabSerializer;
        friend class ScriptEngine;
    };
}

#endif //FOX_LEXER_PREFAB_HPP
