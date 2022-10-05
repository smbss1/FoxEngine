//
// Created by samuel on 30/06/2021.
//

#include "Core/Assert.hpp"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "Core/Application.hpp"
#include "Components/ScriptableBehaviour.hpp"
#include "Components.hpp"
#include "SceneSerializer.hpp"
#include "Scripting/ScriptEngine.hpp"

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
#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
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

    SceneSerializer::SceneSerializer(const ref<Scene> &scene) : m_pScene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        FOX_ASSERT(entity.has<IDComponent>())
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); // TODO: Entity ID goes here

        if (entity.has<EntityName>())
        {
            out << YAML::Key << "NameComponent";
            out << YAML::BeginMap; // NameComponent

            auto& name = entity.get<EntityName>().name;
            out << YAML::Key << "Name" << YAML::Value << name;

            out << YAML::EndMap; // NameComponent
        }

        if (entity.has<TransformComponent>())
        {
            auto& oTransform = entity.get<TransformComponent>();

            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            out << YAML::Key << "Position" << YAML::Value << oTransform.position;
            out << YAML::Key << "Rotation" << YAML::Value << oTransform.rotation;
            out << YAML::Key << "Scale" << YAML::Value << oTransform.scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (entity.has<CameraComponent>())
        {
            auto& cameraComp = entity.get<CameraComponent>();
            auto& camera = cameraComp.camera;

            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

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

            out << YAML::Key << "Primary" << YAML::Value << cameraComp.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComp.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }

        if (entity.has<ScriptComponent>())
        {
            auto& scriptComponent = entity.get<ScriptComponent>();

            out << YAML::Key << "ScriptComponent";
            out << YAML::BeginMap; // ScriptComponent
            out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

            // Fields
            ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
            const auto& fields = entityClass->GetFields();
            if (fields.size() > 0)
            {
                out << YAML::Key << "ScriptFields" << YAML::Value;
                auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
                out << YAML::BeginSeq;
                for (const auto& [name, field] : fields)
                {
                    if (entityFields.find(name) == entityFields.end())
                        continue;

                    out << YAML::BeginMap; // ScriptField
                    out << YAML::Key << "Name" << YAML::Value << name;
                    out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

                    out << YAML::Key << "Data" << YAML::Value;
                    ScriptFieldInstance& scriptField = entityFields.at(name);

                    switch (field.Type)
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
                    }
                    out << YAML::EndMap; // ScriptFields
                }
                out << YAML::EndSeq;
            }

            out << YAML::EndMap; // ScriptComponent
        }

        if (entity.has<SpriteRenderer>())
        {
            auto& oSpriteRenderer = entity.get<SpriteRenderer>();

            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            out << YAML::Key << "Color" << YAML::Value << oSpriteRenderer.Color;

            if (oSpriteRenderer.Sprite.get())
                out << YAML::Key << "Sprite" << YAML::Value << oSpriteRenderer.Sprite.get()->GetId();
            else
                out << YAML::Key << "Sprite" << YAML::Value << "";

            out << YAML::EndMap; // SpriteRendererComponent
        }

        if (entity.has<CircleRenderer>())
        {
            auto& component = entity.get<CircleRenderer>();

            out << YAML::Key << "CircleRenderer";
            out << YAML::BeginMap; // CircleRenderer

            out << YAML::Key << "Color" << YAML::Value << component.Color;
            out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
            out << YAML::Key << "Fade" << YAML::Value << component.Fade;

            out << YAML::EndMap; // CircleRenderer
        }

//        if (entity.has<NativeScript>())
//        {
//            auto& oNativeScript = entity.get<NativeScript>();
//
//            out << YAML::KeyCode << "NativeScriptComponent";
//            out << YAML::BeginMap; // NativeScriptComponent
//            for (auto& script : oNativeScript.m_vScripts)
//            {
//                out << YAML::KeyCode << "Script" << YAML::Value << script.first;
//
//                // TODO: Save variables from the script
//                // out << YAML::BeginMap; // Script
//
//                // out << YAML::KeyCode << "ID" <<
//
//                // out << YAML::EndMap; // Script
//            }
//            out << YAML::EndMap; // NativeScriptComponent
//        }

        if (entity.has<Rigidbody2D>())
        {
            auto& rb2d = entity.get<Rigidbody2D>();

            out << YAML::Key << "Rigidbody2D";
            out << YAML::BeginMap; // Rigidbody2D

            out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2d.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;

            out << YAML::EndMap; // Rigidbody2D
        }

        if (entity.has<BoxCollider2D>())
        {
            auto& box = entity.get<BoxCollider2D>();

            out << YAML::Key << "BoxCollider2D";
            out << YAML::BeginMap; // BoxCollider2D

            out << YAML::Key << "Offset" << YAML::Value << box.Offset;
            out << YAML::Key << "Size" << YAML::Value << box.Size;
            out << YAML::Key << "Density" << YAML::Value << box.Density;
            out << YAML::Key << "Friction" << YAML::Value << box.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << box.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << box.RestitutionThreshold;

            out << YAML::EndMap; // BoxCollider2D
        }

        if (entity.has<CircleCollider2D>())
        {
            auto& component = entity.get<CircleCollider2D>();

            out << YAML::Key << "CircleCollider2D";
            out << YAML::BeginMap; // CircleCollider2D

            out << YAML::Key << "Offset" << YAML::Value << component.Offset;
            out << YAML::Key << "Radius" << YAML::Value << component.Radius;
            out << YAML::Key << "Density" << YAML::Value << component.Density;
            out << YAML::Key << "Friction" << YAML::Value << component.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;

            out << YAML::EndMap; // CircleCollider2D
        }

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string &filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_pScene->m_Registry.each([&](auto entityID)
         {
             Entity entity = { entityID, m_pScene.get() };
             if (!entity)
                 return;

             SerializeEntity(out, entity);
         });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
    }

    bool SceneSerializer::Deserialize(const std::string &filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath);
        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        fox::info("Deserializing scene '%'", sceneName);

        auto entities = data["Entities"];
        if (!entities)
            return true;
        for (auto entity : entities)
        {
            uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

            std::string name;
            auto nameComponent = entity["NameComponent"];
            if (nameComponent)
                name = nameComponent["Name"].as<std::string>();

            fox::info("Deserialized entity with ID = %, name = %", uuid, name);

            Entity deserializedEntity = m_pScene->NewEntityWithUUID(uuid, name);

            auto transformComponent = entity["TransformComponent"];
            if (transformComponent)
            {
                // Entities always have transforms
                auto& tc = deserializedEntity.get<TransformComponent>();
                tc.position = transformComponent["Position"].as<glm::vec3>();
                tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
                tc.scale = transformComponent["Scale"].as<glm::vec3>();
            }

            auto cameraComponent = entity["CameraComponent"];
            if (cameraComponent)
            {
                auto& cc = deserializedEntity.add<CameraComponent>();

                auto cameraProps = cameraComponent["Camera"];
                cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                cc.Primary = cameraComponent["Primary"].as<bool>();
                cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
            }

            auto scriptComponent = entity["ScriptComponent"];
            if (scriptComponent)
            {
                auto& sc = deserializedEntity.add<ScriptComponent>();
                sc.ClassName = scriptComponent["ClassName"].as<std::string>();

                auto scriptFields = scriptComponent["ScriptFields"];
                if (scriptFields)
                {
                    ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
                    FOX_ASSERT(entityClass);
                    const auto& fields = entityClass->GetFields();
                    auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

                    for (auto scriptField : scriptFields)
                    {
                        std::string name = scriptField["Name"].as<std::string>();
                        std::string typeString = scriptField["Type"].as<std::string>();
                        ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

                        ScriptFieldInstance& fieldInstance = entityFields[name];

                        // TODO(Yan): turn this assert into Hazelnut log warning
                        FOX_ASSERT(fields.find(name) != fields.end());

                        if (fields.find(name) == fields.end())
                            continue;

                        fieldInstance.Field = fields.at(name);

                        switch (type)
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
                        }
                    }
                }
            }

            auto spriteRendererComponent = entity["SpriteRendererComponent"];
            if (spriteRendererComponent)
            {
                auto& src = deserializedEntity.add<SpriteRenderer>();
                src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
                auto strFilepath = spriteRendererComponent["Sprite"].as<std::string>();
                if (!strFilepath.empty()) {
                    src.Sprite = Texture2D::Create(strFilepath);
                }
            }

            auto circleRenderer = entity["CircleRenderer"];
            if (circleRenderer)
            {
                auto& src = deserializedEntity.add<CircleRenderer>();
                src.Color = circleRenderer["Color"].as<glm::vec4>();
                src.Thickness = circleRenderer["Thickness"].as<float>();
                src.Fade = circleRenderer["Fade"].as<float>();
            }

//            auto oNativeScriptComponent = entity["NativeScriptComponent"];
//            if (oNativeScriptComponent)
//            {
//                auto& src = deserializedEntity.add<NativeScript>();
//
//                for (auto script : oNativeScriptComponent)
//                {
//                    auto tag = script.second.as<std::size_t>();
//                    ScriptCreator func = m_pScene->GetApp().GetScripts()[tag];
//                    src.add(tag, func());
//                }
//            }

            auto rigidbody2DComponent = entity["Rigidbody2D"];
            if (rigidbody2DComponent)
            {
                auto& rb2d = deserializedEntity.add<Rigidbody2D>();
                rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
                rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
            }

            auto boxCollider2DComponent = entity["BoxCollider2D"];
            if (boxCollider2DComponent)
            {
                auto& bc2d = deserializedEntity.add<BoxCollider2D>();
                bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
                bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
                bc2d.Density = boxCollider2DComponent["Density"].as<float>();
                bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
                bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
            }

            auto circleCollider2DComponent = entity["CircleCollider2D"];
            if (circleCollider2DComponent)
            {
                auto& cc2d = deserializedEntity.add<CircleCollider2D>();
                cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
                cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
                cc2d.Density = circleCollider2DComponent["Density"].as<float>();
                cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
                cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
                cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
    {
        FOX_CORE_ASSERT(false, "");
        return false;
    }
}
