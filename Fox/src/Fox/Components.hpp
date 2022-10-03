

#ifndef FOX_COMPONENTS_HPP_
#define FOX_COMPONENTS_HPP_

#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Components/SpriteRenderer.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/EntityName.hpp"
#include "Components/NativeScript.hpp"
//#include "Components/Animator/AnimationPlayer.hpp"
#include "Components/Rigidbody2D.hpp"
#include "Components/IDComponent.hpp"


// REFLECT_EXTERN(Component);
// REFLECT_EXTERN(fox::TransformComponent);
// REFLECT_EXTERN(SpriteRenderer);
// REFLECT_EXTERN(CameraComponent);
// REFLECT_EXTERN(fox::AnimationPlayer);

namespace fox
{
    template<typename... Component>
    struct ComponentGroup
    {
    };

    using AllComponents =
        ComponentGroup<
            TransformComponent,
            SpriteRenderer,
            CircleRenderer,
            CameraComponent,
            NativeScript,
            Rigidbody2D,
            BoxCollider2D,
            CircleCollider2D
        >;
}

#endif
