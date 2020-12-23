
#include "Node.hpp"

namespace Fox
{
    Node::Node(Engine* pEngine) : m_pEngine(pEngine)
    {
        m_pParent = nullptr;
    }

    Node::~Node()
    {
    }

    void Node::AddChild(Node* pChild)
    {
        m_vChilds.push_back(pChild);
    }

    void Node::SetParent(Node* pParent)
    {
        m_pParent = pParent;
    }

    Node* Node::GetParent()
    {
        return m_pParent;
    }

    void Node::SetTransform(Transform oTransform)
    {
        // m_oWorldTransform.setPosition(pTransform.getPosition());
        // m_oWorldTransform.setRotation(pTransform.getRotation());
        // m_oWorldTransform.setScale(pTransform.getScale());

        m_oWorldTransform = oTransform;
    }

    void Node::SetPosition(Vec2 oPosition)
    {
        // m_oWorldTransform.transformPoint(oPosition.m_oSfVec);
        m_oWorldTransform.setPosition(oPosition.m_oSfVec);
    }

    void Node::SetRotation(Vec2 oRotation)
    {
        // m_oWorldTransform->rotation = oRotation;
    }

    void Node::SetScale(Vec2 oScale)
    {
        // m_oWorldTransform.scale(oScale.x, oScale.y);
        m_oWorldTransform.setScale(oScale.m_oSfVec);
    }

    void Node::UpdateTransform()
    {
        if(m_pParent) {
            //This  node  has a parent
            m_oWorldTransform = m_pParent->m_oWorldTransform * m_oLocalTransform;
        }
        else{ //Root node , world  transform  is  local  transform!
            m_oWorldTransform = m_oLocalTransform;
        }

        for (auto& pChild : m_vChilds)
        {
            pChild->UpdateTransform();
        }
    }
} // namespace Fox