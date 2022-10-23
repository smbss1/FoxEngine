//
// Created by samuel on 07/10/22.
//

#include <iostream>
#include "Reflection.hpp"
#include "Reflection/Core/Reflect.hpp"
#include "Components.hpp"
#include "Scripting/ScriptEngine.hpp"

namespace fox
{
    void Reflection::Init()
    {
        Reflect::Reflect<int>("Int");
        Reflect::Reflect<long>("Long");
        Reflect::Reflect<bool>("Bool");
        Reflect::Reflect<char>("Char");
        Reflect::Reflect<std::byte>("Byte");
        Reflect::Reflect<int16_t>("Short");
        Reflect::Reflect<int32_t>("Int");
        Reflect::Reflect<int64_t>("Long");
        Reflect::Reflect<uint16_t>("UShort");
        Reflect::Reflect<uint32_t>("UInt");
        Reflect::Reflect<uint64_t>("ULong");
        Reflect::Reflect<uint8_t>("UByte");

        Reflect::Reflect<glm::vec2>("Vector2");
        Reflect::Reflect<glm::vec3>("Vector3");
        Reflect::Reflect<glm::vec4>("Vector4");

        Reflect::Reflect<float>("Float")
            .AddConversion<int>();

        Reflect::Reflect<double>("Double")
            .AddConversion<int>();

        Reflect::Reflect<const char *>("cstring")
            .AddConversion<std::string>();

//        Reflect::Reflect<Player>("Player")
//            .AddConstructor<>()
//            .AddConstructor<int>()
//            .AddConstructor<int, const std::string &>()
//            .AddConstructor < Vector3D const & > ()
//            .AddDataMember<&Player::SetId, &Player::GetId>("id")
//            .AddDataMember<&Player::SetName, &Player::GetName>("name")
//            .AddDataMember(&Player::health, "health")
//            .AddDataMember<&Player::SetPosition, &Player::GetPosition>("position")
//            .AddMemberFunction(&Player::Print, "Print")
//            .AddMemberFunction(&Player::SayHello, "SayHello");

        // Transform
        Reflect::Reflect<TransformComponent>("Transform")
            .AddConstructor<>()
            .AddDataMember(&TransformComponent::position, "Position")
            .AddDataMember<&TransformComponent::SetRotation, &TransformComponent::GetRotation>("Rotation")
            .AddDataMember(&TransformComponent::scale, "Scale");

        // Sprite Renderer
        Reflect::Reflect<SpriteRenderer>("SpriteRenderer")
            .AddConstructor<>()
            .AddDataMember(&SpriteRenderer::Sprite, "Sprite")
            .AddDataMember(&SpriteRenderer::Color, "Color")
            .AddDataMember(&SpriteRenderer::TilingFactor, "Tiling Factor");

        // Circle Renderer
        Reflect::Reflect<CircleRenderer>("CircleRenderer")
            .AddConstructor<>()
            .AddDataMember(&CircleRenderer::Color, "Color")
            .AddDataMember(&CircleRenderer::Thickness, "Thickness")
            .AddDataMember(&CircleRenderer::Fade, "Fade");

        // Box Collider2D
        Reflect::Reflect<BoxCollider2D>("BoxCollider2D")
            .AddConstructor<>()
            .AddDataMember(&BoxCollider2D::Offset, "Offset")
            .AddDataMember(&BoxCollider2D::Size, "Size")
            .AddDataMember(&BoxCollider2D::Density, "Density")
            .AddDataMember(&BoxCollider2D::Friction, "Friction")
            .AddDataMember(&BoxCollider2D::Restitution, "Restitution")
            .AddDataMember(&BoxCollider2D::RestitutionThreshold, "Restitution Threshold");

        // Circle Collider 2D
        Reflect::Reflect<CircleCollider2D>("CircleCollider2D")
            .AddConstructor<>()
            .AddDataMember(&CircleCollider2D::Offset, "Offset")
            .AddDataMember(&CircleCollider2D::Radius, "Radius")
            .AddDataMember(&CircleCollider2D::Density, "Density")
            .AddDataMember(&CircleCollider2D::Friction, "Friction")
            .AddDataMember(&CircleCollider2D::Restitution, "Restitution")
            .AddDataMember(&CircleCollider2D::RestitutionThreshold, "Restitution Threshold");

        // Rigidbody 2D
        Reflect::Reflect<Rigidbody2D>("Rigidbody2D")
            .AddConstructor<>()
            .AddDataMember(&Rigidbody2D::Type, "Body Type")
            .AddDataMember(&Rigidbody2D::FixedRotation, "Fixed Rotation")
            .AddDataMember(&Rigidbody2D::GravityScale, "GravityScale");

        // Camera
//        Reflect::Reflect<CameraComponent>("Camera")
//            .AddConstructor<>()
//            .AddDataMember(&CameraComponent::Primary, "Primary")
//            .AddDataMember(&CameraComponent::FixedAspectRatio, "Fixed Rotation");

//        Reflect::Reflect<ScriptFieldInstance>("ScriptFieldInstance")
//            .AddConstructor<>()
//            .AddDataMember<&ScriptFieldInstance::SetValue, &ScriptFieldInstance::>("Value");
    }
}
