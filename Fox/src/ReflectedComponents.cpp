//
// Created by samuel on 12/07/2021.
//

#include "ImGuiVisitor.hpp"

#include <rttr/registration>
#include "Components.hpp"

RTTR_REGISTRATION
{
    rttr::registration::class_<fox::TransformComponent>("Transform")
    ;

    rttr::registration::class_<SpriteRenderer>("SpriteRenderer")
    ;

    rttr::registration::class_<CameraComponent>("Camera")
    ;

    rttr::registration::class_<fox::AnimationPlayer>("AnimationPlayer")
        .constructor<>()
    ;
}
