
#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

#include "Math/Vec2.hpp"

class Transform
{
public:
    Vec2 position;
    Vec2 rotation;
    Vec2 scale;
    Vec2 origin;

    float             m_rotation;                   //!< Orientation of the object, in degrees
    mutable sf::Transform m_transform;                  //!< Combined transformation of the object
    mutable bool      m_transformNeedUpdate;        //!< Does the transform need to be recomputed?
    mutable sf::Transform m_inverseTransform;           //!< Combined transformation of the object
    mutable bool      m_inverseTransformNeedUpdate; //!< Does the transform need to be recomputed?

    Transform* parent;

    explicit Transform() :
    origin                    (0, 0),
    position                  (0, 0),
    m_rotation                  (0),
    scale                     (1, 1),
    m_transform                 (),
    m_transformNeedUpdate       (true),
    m_inverseTransform          (),
    m_inverseTransformNeedUpdate(true)
    {
    }

    explicit Transform(Vec2 position) :
    origin                    (0, 0),
    position                  (position),
    m_rotation                  (0),
    scale                     (1, 1),
    m_transform                 (),
    m_transformNeedUpdate       (true),
    m_inverseTransform          (),
    m_inverseTransformNeedUpdate(true)
    {
    }

    explicit Transform(Vec2 position, Vec2 scale) :
    origin                    (0, 0),
    position                  (position),
    m_rotation                  (0),
    scale                     (scale),
    m_transform                 (),
    m_transformNeedUpdate       (true),
    m_inverseTransform          (),
    m_inverseTransformNeedUpdate(true)
    {
    }

    explicit Transform(const sf::Transform& oTransform) :
    origin                    (0, 0),
    position                  (0, 0),
    m_rotation                  (0),
    scale                     (1, 1),
    m_transform                 (oTransform),
    m_transformNeedUpdate       (true),
    m_inverseTransform          (),
    m_inverseTransformNeedUpdate(true)
    {
    }

    ~Transform()
    {
    }

    void setPosition(float x, float y)
    {
        position.x = x;
        position.y = y;
        m_transformNeedUpdate = true;
        m_inverseTransformNeedUpdate = true;
    }

    void setPosition(const sf::Vector2f& position)
    {
        setPosition(position.x, position.y);
    }

    void setRotation(float angle)
    {
        m_rotation = static_cast<float>(fmod(angle, 360));
        if (m_rotation < 0)
            m_rotation += 360.f;

        m_transformNeedUpdate = true;
        m_inverseTransformNeedUpdate = true;
    }

    void setScale(float factorX, float factorY)
    {
        scale.x = factorX;
        scale.y = factorY;
        m_transformNeedUpdate = true;
        m_inverseTransformNeedUpdate = true;
    }

    void setScale(const sf::Vector2f& factors)
    {
        setScale(factors.x, factors.y);
    }

    void setOrigin(float x, float y)
    {
        origin.x = x;
        origin.y = y;
        m_transformNeedUpdate = true;
        m_inverseTransformNeedUpdate = true;
    }

    void setOrigin(const sf::Vector2f& origin)
    {
        setOrigin(origin.x, origin.y);
    }

    float getRotation() const
    {
        return m_rotation;
    }

    const sf::Transform& getTransform() const
    {
        // Recompute the combined transform if needed
        if (m_transformNeedUpdate)
        {
            float angle  = -m_rotation * 3.141592654f / 180.f;
            float cosine = static_cast<float>(std::cos(angle));
            float sine   = static_cast<float>(std::sin(angle));
            float sxc    = scale.x * cosine;
            float syc    = scale.y * cosine;
            float sxs    = scale.x * sine;
            float sys    = scale.y * sine;
            float tx     = -origin.x * sxc - origin.y * sys + position.x;
            float ty     =  origin.x * sxs - origin.y * syc + position.y;

            m_transform = sf::Transform( sxc, sys, tx,
                                        -sxs, syc, ty,
                                        0.f, 0.f, 1.f);
            m_transformNeedUpdate = false;
        }

        return m_transform;
    }

    const sf::Transform& getInverseTransform() const
    {
        // Recompute the inverse transform if needed
        if (m_inverseTransformNeedUpdate)
        {
            m_inverseTransform = getTransform().getInverse();
            m_inverseTransformNeedUpdate = false;
        }

        return m_inverseTransform;
    }

    Transform operator *(const Transform& oTransform) const
    {
        return Transform(getTransform() * oTransform.getTransform());
    }

    JS_OBJECT(JS_MEMBER(position),
            JS_MEMBER(rotation),
            JS_MEMBER(scale));
};