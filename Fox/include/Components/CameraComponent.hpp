//
// Created by samuel on 29/06/2021.
//

#ifndef FOXENGINE_CAMERACOMPONENT_HPP
#define FOXENGINE_CAMERACOMPONENT_HPP

#include <SceneCamera.hpp>

struct CameraComponent
{
    fox::SceneCamera camera;
    bool Primary = true; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;

};

#endif //FOXENGINE_CAMERACOMPONENT_HPP
