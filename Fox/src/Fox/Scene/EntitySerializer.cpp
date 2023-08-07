
#include "EntitySerializer.hpp"
#include "Core/Assert.hpp"
#include "Core/Application.hpp"
#include "Components/ScriptableBehaviour.hpp"
#include "Components.hpp"
#include "SceneSerializer.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Asset/AssetManager.hpp"
#include "Utils/NameUtils.hpp"
#include "Physics2D.hpp"

#include <fstream>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<fox::UUID>
    {
        static Node encode(const fox::UUID& uuid)
        {
            Node node;
            node.push_back((uint64_t)uuid);
            return node;
        }

        static bool decode(const Node& node, fox::UUID& uuid)
        {
            uuid = node.as<uint64_t>();
            return true;
        }
    };
}

namespace fox
{
//#define FOX_SERIALIZE_PROPERTY(propName, propVal, outputNode) outputNode << YAML::Key << #propName << YAML::Value << propVal
#define FOX_DESERIALIZE_PROPERTY(propName, destination, node) if (node[#propName]) try { destination = node[#propName].as<decltype(destination)>(); } catch(YAML::RepresentationException& e) { FOX_ERROR(e.what()); }
#define FOX_DESERIALIZE_PROPERTY_DEF(propName, destination, node, defaultValue) if (node[#propName]) destination = node[#propName].as<decltype(destination)>(); else destination = defaultValue

#define WRITE_SCRIPT_FIELD(FieldType, Type)       \
            case ScriptFieldType::FieldType:          \
                out << scriptField.GetValue<Type>();  \
                break

    #define READ_SCRIPT_FIELD(FieldType, Type)             \
        case ScriptFieldType::FieldType:                   \
        {                                                  \
            Type data = scriptField["Data"].as<Type>();    \
            storage->SetValue(data);                  \
            break;                                         \
        }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    static std::string RigidBody2DBodyTypeToString(Rigidbody2D::BodyType bodyType)
    {
        switch (bodyType)
        {
            case Rigidbody2D::BodyType::Static:    return "Static";
            case Rigidbody2D::BodyType::Dynamic:   return "Dynamic";
            case Rigidbody2D::BodyType::Kinematic: return "Kinematic";
        }

        FOX_ASSERT(false, "Unknown body type");
        return {};
    }

    static Rigidbody2D::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
    {
        if (bodyTypeString == "Static")    return Rigidbody2D::BodyType::Static;
        if (bodyTypeString == "Dynamic")   return Rigidbody2D::BodyType::Dynamic;
        if (bodyTypeString == "Kinematic") return Rigidbody2D::BodyType::Kinematic;

        FOX_ASSERT(false, "Unknown body type");
        return Rigidbody2D::BodyType::Static;
    }

    template<typename Component>
	void SerializeEntityComponent(YAML::Emitter&, Entity, Component&) = delete;

	template<typename Component>
	void DeserializeEntryComponent(YAML::Node& entity, Entity&, Component&) = delete;

	template<>
	void SerializeEntityComponent<NameComponent>(YAML::Emitter& out, Entity entity, NameComponent& component)
	{
        out << YAML::Key << "Name" << YAML::Value << component.name;
    }

	template<>
	void SerializeEntityComponent<TransformComponent>(YAML::Emitter& out, Entity entity, TransformComponent& component)
	{
        out << YAML::Key << "Position" << YAML::Value << component.position;
        out << YAML::Key << "Rotation" << YAML::Value << component.GetRotation();
        out << YAML::Key << "Scale" << YAML::Value << component.scale;
	}

	template<>
    [[maybe_unused]] void DeserializeEntryComponent<TransformComponent>(YAML::Node& data, Entity& deserializedEntity, TransformComponent& component)
	{
        // Entities always have transforms
        component.position = data["Position"].as<glm::vec3>();
        component.SetRotation(data["Rotation"].as<glm::vec3>());
        component.scale = data["Scale"].as<glm::vec3>();
	}

    template<>
    void SerializeEntityComponent<HierarchyComponent>(YAML::Emitter& out, Entity entity, HierarchyComponent& component)
    {
        out << YAML::Key << "Parent" << YAML::Value << component.ParentID;

        out << YAML::Key << "Children";
        out << YAML::Value << YAML::BeginSeq;

        for (auto child : component.Children)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Handle" << YAML::Value << child;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }

    template<>
    [[maybe_unused]] void DeserializeEntryComponent<HierarchyComponent>(YAML::Node& data, Entity& deserializedEntity, HierarchyComponent& component)
    {
        uint64_t parentHandle = data["Parent"] ? data["Parent"].as<uint64_t>() : 0;
        component.ParentID = parentHandle;

        auto children = data["Children"];
        if (children)
        {
            for (auto child : children)
            {
                uint64_t childHandle;
                FOX_DESERIALIZE_PROPERTY(Handle, childHandle, child);
                component.Children.emplace_back(childHandle);
            }
        }
    }

    template<>
	void SerializeEntityComponent<CameraComponent>(YAML::Emitter& out, Entity entity, CameraComponent& component)
	{
        auto& camera = component.camera;

        out << YAML::Key << "Camera" << YAML::Value;
        out << YAML::BeginMap; // Camera
        out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
        out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
        out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
        out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << component.Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;
	}

	template<>
    [[maybe_unused]] void DeserializeEntryComponent<CameraComponent>(YAML::Node& data, Entity& deserializedEntity, CameraComponent& component)
	{
        auto cameraProps = data["Camera"];
        component.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

        component.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
        component.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
        component.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

        component.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
        component.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
        component.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

        component.Primary = data["Primary"].as<bool>();
        component.FixedAspectRatio = data["FixedAspectRatio"].as<bool>();
	}

	template<>
	void SerializeEntityComponent<SpriteRenderer>(YAML::Emitter& out, Entity entity, SpriteRenderer& component)
	{
        out << YAML::Key << "Color" << YAML::Value << component.Color;

        if (AssetManager::IsAssetHandleValid(component.Sprite))
            out << YAML::Key << "Sprite" << YAML::Value << component.Sprite;
	}

	template<>
	void DeserializeEntryComponent<SpriteRenderer>(YAML::Node& data, Entity& deserializedEntity, SpriteRenderer& component)
	{
        component.Color = data["Color"].as<glm::vec4>();
        FOX_DESERIALIZE_PROPERTY(Color, component.Color, data);

        AssetHandle handle;
        FOX_DESERIALIZE_PROPERTY(Sprite, handle, data);
        if (AssetManager::IsAssetHandleValid(handle))
            component.Sprite = handle;
	}

	template<>
	void SerializeEntityComponent<CircleRenderer>(YAML::Emitter& out, Entity entity, CircleRenderer& component)
	{
        out << YAML::Key << "Color" << YAML::Value << component.Color;
        out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
        out << YAML::Key << "Fade" << YAML::Value << component.Fade;
	}

	template<>
	void DeserializeEntryComponent<CircleRenderer>(YAML::Node& data, Entity& deserializedEntity, CircleRenderer& component)
	{
        component.Color = data["Color"].as<glm::vec4>();
        component.Thickness = data["Thickness"].as<float>();
        component.Fade = data["Fade"].as<float>();
	}

	template<>
	void SerializeEntityComponent<Rigidbody2D>(YAML::Emitter& out, Entity entity, Rigidbody2D& component)
	{
        out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(component.Type);
        out << YAML::Key << "FixedRotation" << YAML::Value << component.FixedRotation;
        out << YAML::Key << "GravityScale" << YAML::Value << component.GravityScale;
        out << YAML::Key << "LayerID" << YAML::Value << component.LayerID;
	}

	template<>
	void DeserializeEntryComponent<Rigidbody2D>(YAML::Node& data, Entity& deserializedEntity, Rigidbody2D& component)
	{
        std::string type;
        FOX_DESERIALIZE_PROPERTY_DEF(BodyType, type, data, "Static");
        component.Type = RigidBody2DBodyTypeFromString(type);

        FOX_DESERIALIZE_PROPERTY_DEF(FixedRotation, component.FixedRotation, data, false);
        FOX_DESERIALIZE_PROPERTY_DEF(GravityScale, component.GravityScale, data, 1);
        FOX_DESERIALIZE_PROPERTY_DEF(LayerID, component.LayerID, data, 0);
        if (!Physics2D::LayerIsValid(component.LayerID))
        {
            FOX_WARN_TAG("EntitySerializer", "LayerID (%) is not valid. Set is value to default", component.LayerID);
            component.LayerID = Physics2D::DefaultLayer;
        }
	}

	template<>
	void SerializeEntityComponent<BoxCollider2D>(YAML::Emitter& out, Entity entity, BoxCollider2D& component)
	{
        out << YAML::Key << "Offset" << YAML::Value << component.Offset;
        out << YAML::Key << "Size" << YAML::Value << component.Size;
        out << YAML::Key << "Density" << YAML::Value << component.Density;
        out << YAML::Key << "Friction" << YAML::Value << component.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
	}

	template<>
	void DeserializeEntryComponent<BoxCollider2D>(YAML::Node& data, Entity& deserializedEntity, BoxCollider2D& component)
	{
        component.Offset = data["Offset"].as<glm::vec2>();
        component.Size = data["Size"].as<glm::vec2>();
        component.Density = data["Density"].as<float>();
        component.Friction = data["Friction"].as<float>();
        component.Restitution = data["Restitution"].as<float>();
        component.RestitutionThreshold = data["RestitutionThreshold"].as<float>();
	}

    template<>
	void SerializeEntityComponent<CircleCollider2D>(YAML::Emitter& out, Entity entity, CircleCollider2D& component)
	{
        out << YAML::Key << "Offset" << YAML::Value << component.Offset;
        out << YAML::Key << "Radius" << YAML::Value << component.Radius;
        out << YAML::Key << "Density" << YAML::Value << component.Density;
        out << YAML::Key << "Friction" << YAML::Value << component.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
	}

	template<>
	void DeserializeEntryComponent<CircleCollider2D>(YAML::Node& data, Entity& deserializedEntity, CircleCollider2D& component)
	{
        component.Offset = data["Offset"].as<glm::vec2>();
        component.Radius = data["Radius"].as<float>();
        component.Density = data["Density"].as<float>();
        component.Friction = data["Friction"].as<float>();
        component.Restitution = data["Restitution"].as<float>();
        component.RestitutionThreshold = data["RestitutionThreshold"].as<float>();
	}

//	template<>
//	void SerializeEntityComponent<NativeScript>(YAML::Emitter& out, Entity entity, NativeScript& component)
//	{
////        if (entity.has<NativeScript>())
////        {            FOX_DESERIALIZE_PROPERTY(ClassName, sc.ClassName, data);
//
////            auto& oNativeScript = entity.get<NativeScript>();
////
////            out << YAML::KeyCode << "NativeScriptComponent";
////            out << YAML::BeginMap; // NativeScriptComponent
////            for (auto& script : oNativeScript.m_vScripts)
////            {
////                out << YAML::KeyCode << "Script" << YAML::Value << script.first;
////
////                // TODO: Save variables from the script
////                // out << YAML::BeginMap; // Script
////
////                // out << YAML::KeyCode << "ID" <<
////
////                // out << YAML::EndMap; // Script
////            }
////            out << YAML::EndMap; // NativeScriptComponent
////        }
//	}

//	template<>
//	void DeserializeEntryComponent<NativeScript>(YAML::Node& data, Entity& deserializedEntity, NativeScript& component)
//	{
////            auto oNativeScriptComponent = entity["NativeScriptComponent"];
////            if (oNativeScriptComponent)
////            {
////                auto& src = deserializedEntity.add<NativeScript>();
////
////                for (auto script : oNativeScriptComponent)
////                {
////                    auto tag = script.second.as<std::size_t>();
////                    ScriptCreator func = m_pScene->GetApp().GetScripts()[tag];
////                    src.add(tag, func());
////                }
////            }
//	}

    template<>
	void SerializeEntityComponent<ScriptComponent>(YAML::Emitter& out, Entity entity, ScriptComponent& component)
	{
        out << YAML::Key << "ClassName" << YAML::Value << component.ClassName;

        // Fields
        ManagedClass* entityClass = ScriptEngine::GetEntityClass(component.ClassName);
        const auto& fields = entityClass->Fields;
        if (fields.empty())
            return;
        out << YAML::Key << "ScriptFields" << YAML::Value << YAML::BeginSeq;
        for (const auto fieldID : fields)
        {
            ManagedField* fieldInfo = ScriptCache::GetFieldByID(fieldID);
            if (!fieldInfo->Type.IsValid())
            {
                FOX_CORE_WARN("C# field % not serialized, unknown type", fieldInfo->FullName);
                continue;
            }

            if (!fieldInfo->IsWritable())
                continue;

            FieldStorage& scriptField = *ScriptEngine::GetFieldStorage(entity, fieldID);

            out << YAML::BeginMap; // ScriptField
            out << YAML::Key << "ID" << YAML::Value << fieldInfo->ID;
            out << YAML::Key << "Name" << YAML::Value << fieldInfo->FullName;
            out << YAML::Key << "Type" << YAML::Value << fieldInfo->Type.TypeEncoding;

            out << YAML::Key << "Data" << YAML::Value;

            switch (fieldInfo->Type.NativeType)
            {
                WRITE_SCRIPT_FIELD(Float,   float     );
                WRITE_SCRIPT_FIELD(Double,  double    );
                WRITE_SCRIPT_FIELD(Bool,    bool      );
                WRITE_SCRIPT_FIELD(Char,    char      );
                WRITE_SCRIPT_FIELD(Byte,    int8_t    );
                WRITE_SCRIPT_FIELD(Short,   int16_t   );
                WRITE_SCRIPT_FIELD(Int,     int32_t   );
                WRITE_SCRIPT_FIELD(Long,    int64_t   );
                WRITE_SCRIPT_FIELD(UByte,   uint8_t   );
                WRITE_SCRIPT_FIELD(UShort,  uint16_t  );
                WRITE_SCRIPT_FIELD(UInt,    uint32_t  );
                WRITE_SCRIPT_FIELD(ULong,   uint64_t  );
                WRITE_SCRIPT_FIELD(Vector2, glm::vec2 );
                WRITE_SCRIPT_FIELD(Vector3, glm::vec3 );
                WRITE_SCRIPT_FIELD(Vector4, glm::vec4 );
                WRITE_SCRIPT_FIELD(Entity,  UUID      );
                WRITE_SCRIPT_FIELD(Prefab,  AssetHandle);
                WRITE_SCRIPT_FIELD(AssetHandle,  AssetHandle);
            }
            out << YAML::EndMap; // ScriptFields
        }
        out << YAML::EndSeq;
	}

	template<>
	void DeserializeEntryComponent<ScriptComponent>(YAML::Node& data, Entity& deserializedEntity, ScriptComponent& component)
	{
        FOX_DESERIALIZE_PROPERTY(ClassName, component.ClassName, data);

//            std::string moduleName = scriptComponent["ModuleName"] ? scriptComponent["ModuleName"].as<std::string>("") : "";
        ScriptEngine::InitializeScriptEntity(deserializedEntity);

        auto scriptFields = data["ScriptFields"];
        if (!scriptFields)
            return;
        for (auto scriptField : scriptFields)
        {
            UUID id;
            FOX_DESERIALIZE_PROPERTY_DEF(ID, id, scriptField, 0);

            std::string name;
            FOX_DESERIALIZE_PROPERTY(Name, name, scriptField);

            ManagedType type;
            ManagedField* fieldData = ScriptCache::GetFieldByID(id);
            if (fieldData != nullptr)
                type = fieldData->Type;

            Ref<FieldStorage> storage = ScriptEngine::GetFieldStorage(deserializedEntity, id);
            if (storage == nullptr)
            {
                // TODO(Yan): turn this assert into FoxEditor log warning
                FOX_WARN("Serialized C# field % doesn't exist in script cache! This could be because the script field no longer exists or because it's been renamed.", name);
                continue;
            }

            switch (type.NativeType)
            {
                READ_SCRIPT_FIELD(Float,   float     );
                READ_SCRIPT_FIELD(Double,  double    );
                READ_SCRIPT_FIELD(Bool,    bool      );
                READ_SCRIPT_FIELD(Char,    char      );
                READ_SCRIPT_FIELD(Byte,    int8_t    );
                READ_SCRIPT_FIELD(Short,   int16_t   );
                READ_SCRIPT_FIELD(Int,     int32_t   );
                READ_SCRIPT_FIELD(Long,    int64_t   );
                READ_SCRIPT_FIELD(UByte,   uint8_t   );
                READ_SCRIPT_FIELD(UShort,  uint16_t  );
                READ_SCRIPT_FIELD(UInt,    uint32_t  );
                READ_SCRIPT_FIELD(ULong,   uint64_t  );
                READ_SCRIPT_FIELD(Vector2, glm::vec2 );
                READ_SCRIPT_FIELD(Vector3, glm::vec3 );
                READ_SCRIPT_FIELD(Vector4, glm::vec4 );
                READ_SCRIPT_FIELD(Entity,  UUID      );
                READ_SCRIPT_FIELD(Prefab,  AssetHandle);
                READ_SCRIPT_FIELD(AssetHandle, AssetHandle);
            }
        }
	}

    template<>
    void SerializeEntityComponent<PrefabComponent>(YAML::Emitter& out, Entity entity, PrefabComponent& component)
    {
        out << YAML::Key << "Prefab" << YAML::Value << component.PrefabID;
        out << YAML::Key << "Entity" << YAML::Value << component.EntityID;
    }

    template<>
    void DeserializeEntryComponent<PrefabComponent>(YAML::Node& data, Entity& deserializedEntity, PrefabComponent& component)
    {
        FOX_DESERIALIZE_PROPERTY(Entity, component.EntityID, data);
        FOX_DESERIALIZE_PROPERTY(Prefab, component.PrefabID, data);
    }

    template<>
    void SerializeEntityComponent<ParticleSystem>(YAML::Emitter& out, Entity entity, ParticleSystem& component)
    {
        out << YAML::Key << "PlayOnStart" << YAML::Value << component.Play;
        out << YAML::Key << "MaxParticles" << YAML::Value << component.MaxParticles;
        out << YAML::Key << "LifeTime" << YAML::Value << component.ParticleSettings.LifeTime;
        out << YAML::Key << "RotationSpeed" << YAML::Value << component.ParticleSettings.RotationSpeed;
        out << YAML::Key << "Speed" << YAML::Value << component.ParticleSettings.Speed;
        out << YAML::Key << "Velocity" << YAML::Value << component.ParticleSettings.Velocity;
        out << YAML::Key << "VelocityVariation" << YAML::Value << component.ParticleSettings.VelocityVariation;
        out << YAML::Key << "ColorBegin" << YAML::Value << component.ParticleSettings.ColorBegin;
        out << YAML::Key << "ColorEnd" << YAML::Value << component.ParticleSettings.ColorEnd;
        out << YAML::Key << "SizeBegin" << YAML::Value << component.ParticleSettings.SizeBegin;
        out << YAML::Key << "SizeEnd" << YAML::Value << component.ParticleSettings.SizeEnd;
        out << YAML::Key << "SizeVariation" << YAML::Value << component.ParticleSettings.SizeVariation;
        out << YAML::Key << "RateOverTime" << YAML::Value << component.ParticleSettings.RateOverTime;
    }

    template<>
    void DeserializeEntryComponent<ParticleSystem>(YAML::Node& data, Entity& deserializedEntity, ParticleSystem& component)
    {
        FOX_DESERIALIZE_PROPERTY(PlayOnStart, component.Play, data);
        FOX_DESERIALIZE_PROPERTY(LifeTime, component.ParticleSettings.LifeTime, data);
        FOX_DESERIALIZE_PROPERTY(RotationSpeed, component.ParticleSettings.RotationSpeed, data);
        FOX_DESERIALIZE_PROPERTY(Speed, component.ParticleSettings.Speed, data);
        FOX_DESERIALIZE_PROPERTY(Velocity, component.ParticleSettings.Velocity, data);
        FOX_DESERIALIZE_PROPERTY(VelocityVariation, component.ParticleSettings.VelocityVariation, data);
        FOX_DESERIALIZE_PROPERTY(ColorBegin, component.ParticleSettings.ColorBegin, data);
        FOX_DESERIALIZE_PROPERTY(ColorEnd, component.ParticleSettings.ColorEnd, data);
        FOX_DESERIALIZE_PROPERTY(SizeBegin, component.ParticleSettings.SizeBegin, data);
        FOX_DESERIALIZE_PROPERTY(SizeEnd, component.ParticleSettings.SizeEnd, data);
        FOX_DESERIALIZE_PROPERTY(SizeVariation, component.ParticleSettings.SizeVariation, data);
        FOX_DESERIALIZE_PROPERTY(RateOverTime, component.ParticleSettings.RateOverTime, data);
    }

    template<>
    void SerializeEntityComponent<Animator>(YAML::Emitter& out, Entity entity, Animator& component)
    {
        out << YAML::Key << "Animations" << YAML::Value << YAML::BeginSeq;
        for (auto& animation : component.Animations)
        {
            out << YAML::BeginMap; // Animation
            out << YAML::Key << "Name" << YAML::Value << animation->Name;
            out << YAML::Key << "Speed" << YAML::Value << animation->Speed;
            out << YAML::Key << "Loop" << YAML::Value << animation->Loop;

            out << YAML::Key << "Frames" << YAML::Value << YAML::BeginSeq;
            {
                for (auto frame: animation->Frames) {
                    out << YAML::BeginMap; // Frame
                    out << YAML::Key << "Texture" << YAML::Value << frame->Texture;

                    if (frame->EventID != UUID::Empty())
                    {
                        out << YAML::Key << "EventID" << YAML::Value << frame->EventID;
                    }
                    out << YAML::EndMap; // Frame
                }
            }
            out << YAML::EndSeq; // Frames

            out << YAML::Key << "Events" << YAML::Value << YAML::BeginSeq;
            {
                for (auto event: animation->Events) {
                    out << YAML::BeginMap; // Event
                    out << YAML::Key << "Name" << YAML::Value << event->Name;
                    out << YAML::Key << "ID" << YAML::Value << event->ID;
                    out << YAML::EndMap; // Event
                }
            }
            out << YAML::EndSeq; // Frames

            out << YAML::EndMap; // Animation
        }
        out << YAML::EndSeq; // Animations
    }

    template<>
    void DeserializeEntryComponent<Animator>(YAML::Node& data, Entity& deserializedEntity, Animator& component)
    {
        for (auto animData: data["Animations"])
        {
            Ref<Animation> animation = new_ref<Animation>();

            FOX_DESERIALIZE_PROPERTY(Speed, animation->Speed, animData);
            FOX_DESERIALIZE_PROPERTY(Loop, animation->Loop, animData);
            FOX_DESERIALIZE_PROPERTY_DEF(Name, animation->Name, animData, "New Animation");

            for (auto eventData: animData["Events"]) {
                Ref<Anim::Event> event = new_ref<Anim::Event>();
                FOX_DESERIALIZE_PROPERTY(Name, event->Name, eventData);
                FOX_DESERIALIZE_PROPERTY(ID, event->ID, eventData);
                animation->Events.push_back(event);
            }

            uint32_t i = 0;
            for (auto frameData: animData["Frames"]) {
                Ref<Frame> frame = new_ref<Frame>();
                FOX_DESERIALIZE_PROPERTY(Texture, frame->Texture, frameData);
                FOX_DESERIALIZE_PROPERTY(EventID, frame->EventID, frameData);
                if (frame->EventID != UUID::Empty())
                {
                    Ref<Anim::Event> event = animation->FindEvent(frame->EventID);
                    if (!event) {
                        FOX_WARN_TAG("EntitySerializer", "Unknown animation event (%) at frame index(%)", frame->EventID, i);
                        frame->EventID = UUID::Empty();
                    }
                }
                animation->Frames.push_back(frame);
                i++;
            }

            animation->SetCurrentIndex(0);
            component.Animations.push_back(animation);
        }
        // init the current index so the current animation is also updated
        component.Init();
    }

	template<typename...Component>
	void SerializeEntityComponents(YAML::Emitter& out, Entity entity)
	{
        ([&out, &entity]()
        {
            if (entity.has<Component>())
            {
                std::string str = name_helper<Component>::name();
                std::string::size_type index = str.find_last_of(':');
                if (index != std::string::npos)
                {
                    str = str.substr(index + 1, str.size());
                }

                auto& component = entity.get<Component>();

                out << YAML::Key << str;
                out << YAML::BeginMap; // Component Name
                SerializeEntityComponent<Component>(out, entity, component);
                out << YAML::EndMap; // Component Name
            }
        }(), ...);
	}

	template<typename...Component>
	void DeserializeEntryComponents(YAML::Node& entity, Entity& deserializedEntity)
	{
        ([&deserializedEntity, &entity]()
        {
            std::string str = name_helper<Component>::name();
            std::string::size_type index = str.find_last_of(':');
            if (index != std::string::npos)
            {
                str = str.substr(index + 1, str.size());
            }

            auto data = entity[str];
            if (data)
            {
                Component* component;
                if (!deserializedEntity.has<Component>())
                    component = &deserializedEntity.add<Component>();
                else
                    component = &deserializedEntity.get<Component>();
                DeserializeEntryComponent<Component>(data, deserializedEntity, *component);
            }
        }(), ...);
	}

	template<typename... PrefixComponent, typename...Component>
	void SerializeEntityComponents(ComponentGroup<Component...>, YAML::Emitter& out, Entity entity)
	{
		SerializeEntityComponents<PrefixComponent..., Component...>(out, entity);
	}

	template<typename... PrefixComponent, typename...Component>
	void DeserializeEntryComponents(ComponentGroup<Component...>, YAML::Node& entity, Entity& deserializedEntity)
	{
		DeserializeEntryComponents<PrefixComponent..., Component...>(entity, deserializedEntity);
	}

	void EntitySerializer::SerializeAllEntityComponents(YAML::Emitter& out, Entity entity)
	{
		SerializeEntityComponents<NameComponent>(AllComponents{}, out, entity);
	}

	void EntitySerializer::DeserializeAllEntryComponents(YAML::Node& entity, Entity& deserializedEntity)
	{
		DeserializeEntryComponents(AllComponents{}, entity, deserializedEntity);
	}

    void EntitySerializer::SerializeEntityAsPrefab(const char* filepath, Entity entity)
    {
        if (entity.has<PrefabComponent>())
        {
            FOX_CORE_ERROR("Entity already has a prefab component!");
            return;
        }

        YAML::Emitter out;

        Ref<Scene> scene = Scene::CreateEmpty();
        std::string name = entity.GetName();
        Entity newEntity = scene->NewEntity(name);

        entity.m_Scene->CopyAllComponentsIfExists(newEntity, entity);

        // Add PrefabComponent
        auto& prefabComp = newEntity.add<PrefabComponent>(UUID(), newEntity.GetUUID());
        out << YAML::BeginMap; // Prefab
        out << YAML::Key << "Prefab" << YAML::Value << prefabComp.PrefabID;
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
        // Serialize components (except IDComponent)
        EntitySerializer::SerializeAllEntityComponents(out, newEntity);
        out << YAML::EndMap; // Entity

        out << YAML::EndSeq;
        out << YAML::EndMap; // Prefab

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    Entity EntitySerializer::DeserializeEntityAsPrefab(const char* filepath, Scene& scene)
    {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Prefab"])
        {
            FOX_CORE_ERROR("Invalid prefab format: %", std::filesystem::path(filepath).filename());
            return {};
        }

        std::string filename = std::filesystem::path(filepath).filename();
        UUID prefabID = data["Prefab"].as<UUID>();
        if (!prefabID)
        {
            FOX_CORE_ERROR("Invalid prefab id : % (%)", filename, prefabID);
            return {};
        }

        FOX_CORE_INFO("Deserializing prefab : % (%)", filename, prefabID);

        Entity root = {};

        auto entities = data["Entities"];
        if (!entities)
        {
            FOX_CORE_ERROR("There are no entities in the prefab % (%) to deserialize!", filename, prefabID);
            return root;
        }

        for (auto entity : entities)
        {
            auto uuid = entity["Entity"].as<uint64_t>();

            std::string name;
            auto nameComponent = entity["NameComponent"];
            if (nameComponent)
                name = nameComponent["Name"].as<std::string>();

            FOX_CORE_INFO("Deserialized entity with ID = %, name = %", uuid, name);

            Entity deserializedEntity = scene.NewEntity(name);

            // Deserialize components (except IDComponent and TagComponent)
            EntitySerializer::DeserializeAllEntryComponents(entity, deserializedEntity);

            if (!root)
                root = deserializedEntity;
        }
        return root;
    }
} // namespace fox
