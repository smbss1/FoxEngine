//
// Created by samuel on 02/10/22.
//

#ifndef FOX_ENGINE_RIGIDBODY2D_HPP
#define FOX_ENGINE_RIGIDBODY2D_HPP

#include "glm/glm.hpp"

struct Rigidbody2D // : public ComponentImpl<Rigidbody2D>
{
    enum class BodyType { Static = 0, Dynamic, Kinematic };
    BodyType Type = BodyType::Static;
    bool FixedRotation = false;

    // Storage for runtime
    void* RuntimeBody = nullptr;

    Rigidbody2D() = default;
    Rigidbody2D(const Rigidbody2D&) = default;
};

struct BoxCollider2D // : public ComponentImpl<BoxCollider2D>
{
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 0.5f, 0.5f };

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    // Storage for runtime
    void* RuntimeFixture = nullptr;

    BoxCollider2D() = default;
    BoxCollider2D(const BoxCollider2D&) = default;
};

struct CircleCollider2D // : public ComponentImpl<CircleCollider2D>
{
    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    // Storage for runtime
    void* RuntimeFixture = nullptr;

    CircleCollider2D() = default;
    CircleCollider2D(const CircleCollider2D&) = default;
};

#endif //FOX_LEXER_RIGIDBODY2D_HPP
