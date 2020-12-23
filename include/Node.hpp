#pragma once

#include <vector>

#include "Components/Transform.hpp"
#include "Engine.hpp"

namespace Fox
{
    class Node
    {
    public:
        Node(Engine* pEngine);
        ~Node();
        void AddChild(Node* pChild);
        void SetParent(Node* pParent);
        Node* GetParent();
        void SetTransform(Transform oTransform);
        void SetPosition(Vec2 oPosition);
        void SetRotation(Vec2 oRotation);
        void SetScale(Vec2 oScale);
        void UpdateTransform();

    public:
        std::vector<Node*> m_vChilds;
        Node* m_pParent;
        Transform m_oLocalTransform;
        Transform m_oWorldTransform;

        Engine* m_pEngine;
    };
} // namespace Fox