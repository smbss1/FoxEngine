//
// Created by samuel on 12/07/2021.
//

// #include <rttr/registration>
// #include "Components.hpp"

// REFLECT(Component)
// {
//     rttr::registration::class_<Component>("Component");
// }

// REFLECT(fox::TransformComponent)
// {
//     rttr::registration::class_<fox::TransformComponent>("Transform")
//         (
//             rttr::metadata("pretty_name", "Transform")
//         )
//         .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
//             .property("Position", &fox::TransformComponent::position)
//             .property("Rotation", &fox::TransformComponent::rotation)
//                     ( rttr::metadata("CONVERT_TO_DEGREE", true) )
//             .property("Scale", &fox::TransformComponent::scale)
//         ;
// }

// REFLECT(SpriteRenderer)
// {
//     rttr::registration::class_<SpriteRenderer>("SpriteRenderer")
//         (
//             rttr::metadata("pretty_name", "Sprite Renderer")
//         )
//             .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
//             .property("Sprite", &SpriteRenderer::m_pSprite)
//                     (
//                             rttr::metadata("tooltip", "This the texture to display on the screen")
//                     )
//             .property("Color", &SpriteRenderer::m_oColor)
//             ;
// }

// REFLECT(CameraComponent)
// {
//     rttr::registration::class_<CameraComponent>("Camera")
//         (
//             rttr::metadata("pretty_name", "Camera")
//         )
//             .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
//             ;
// }

// REFLECT(fox::AnimationPlayer)
// {
//     rttr::registration::class_<fox::AnimationPlayer>("AnimationPlayer")
//         (
//             rttr::metadata("pretty_name", "Animation Player")
//         )
//         .constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
//     ;
// }
