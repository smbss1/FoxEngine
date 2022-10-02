//
// Created by samuel on 29/06/2021.
//

#ifndef FOXENGINE_CAMERACOMPONENT_HPP
#define FOXENGINE_CAMERACOMPONENT_HPP

#include <SceneCamera.hpp>
// #include <rttr/type>
#include "Component.hpp"
#include "Reflection.hpp"

struct CameraComponent : public ComponentImpl<CameraComponent>
{
    // REFLECTABLEV(CameraComponent, Component)

public:
    fox::SceneCamera camera;
    bool Primary = true; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    CameraComponent() = default;
};

#endif //FOXENGINE_CAMERACOMPONENT_HPP
