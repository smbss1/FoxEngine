//
// Created by samuel on 27/10/22.
//

#ifndef FOX_LEXER_ANIMATORSPRITE_HPP
#define FOX_LEXER_ANIMATORSPRITE_HPP

#include "Panel.hpp"
#include "Ecs/Entity.hpp"
#include "Components/SpriteRenderer.hpp"

namespace fox
{
    struct OnSelectedEntityChangeEvent;
    class AnimatorSpritePanel : public Panel
    {
    public:
        AnimatorSpritePanel();
        void OnImGui(bool& isOpen) override;

    private:
        void OnSelectedEntityChange(const OnSelectedEntityChangeEvent& event);

    private:
        Entity m_SelectedEntity;
        Animator* m_Animator = nullptr;
    };
}

#endif //FOX_LEXER_ANIMATORSPRITE_HPP
