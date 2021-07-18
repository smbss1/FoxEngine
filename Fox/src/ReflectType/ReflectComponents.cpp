//
// Created by samuel on 12/07/2021.
//

#include <rttr/registration>
#include "Reflection/ReflectComponents.hpp"
#include "Ecs/World.hpp"

REFLECT(fox::Entity)
{
    rttr::registration::class_<fox::Entity>("Entity")
            .property("ID", &fox::Entity::get_id, &fox::Entity::set_id)
                (
                    rttr::metadata("NO_SERIALIZE", "")
                )
            .property("Components", &fox::Entity::GetAll, &fox::Entity::SetAll)
    ;
}

REFLECT(Component)
{
    rttr::registration::class_<Component>("Component");
}

REFLECT(fox::TransformComponent)
{
    rttr::registration::class_<fox::TransformComponent>("Transform")
        (
            rttr::metadata("pretty_name", "Transform")
        )
        .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
            .property("Position", &fox::TransformComponent::position)
            .property("Rotation", &fox::TransformComponent::rotation)
                    ( rttr::metadata("CONVERT_TO_DEGREE", true) )
            .property("Scale", &fox::TransformComponent::scale)
        ;
}

REFLECT(SpriteRenderer)
{
    rttr::registration::class_<SpriteRenderer>("SpriteRenderer")
        (
            rttr::metadata("pretty_name", "Sprite Renderer")
        )
            .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
            .property("Sprite", &SpriteRenderer::m_pSprite)
                (
                        rttr::metadata("tooltip", "This the texture to display on the screen"),
                        rttr::metadata("NO_SERIALIZE", "")
                )
            .property("Color", &SpriteRenderer::m_oColor)
            .property("Filepath", &SpriteRenderer::GetFilepath, &SpriteRenderer::SetFilepath)
                (
                        rttr::metadata("NO_UI", "")
                )
            ;
}

REFLECT(CameraComponent)
{
    rttr::registration::class_<CameraComponent>("Camera")
        (
            rttr::metadata("pretty_name", "Camera")
        )
        .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
            .property("Primary", &CameraComponent::Primary)
            .property("FixedAspectRatio", &CameraComponent::FixedAspectRatio)
                (
                    rttr::metadata("pretty_name", "Fixed Aspect Ratio")
                )
    ;
}

REFLECT(fox::AnimationPlayer)
{
    rttr::registration::class_<fox::AnimationPlayer>("AnimationPlayer")
        (
            rttr::metadata("pretty_name", "Animation Player")
        )
        .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
    ;
}

REFLECT(fox::EntityName)
{
    rttr::registration::class_<fox::EntityName>("NameComponent")
        .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
                .property("name", &fox::EntityName::name)
    ;
}

REFLECT(fox::EntityTag)
{
    rttr::registration::class_<fox::EntityTag>("TagComponent")
            .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
            .property("tag", &fox::EntityTag::tag)
            ;
}