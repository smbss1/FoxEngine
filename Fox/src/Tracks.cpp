//
// Created by samuel on 17/07/2021.
//

#include <rttr/registration.h>
#include <Assets/Texture.hpp>

#include "Components/Animator/Track.hpp"
#include "glm/glm.hpp"

#define REGISTER(type) \
    rttr::registration::class_<Track<type>>("Track_"#type) \
            .property("frames", &Track<type>::m_vFrames) \
            ; \
    rttr::registration::class_<Frame<type>>("Frame_"#type) \
            .property("time", &Frame<type>::m_fTime) \
            .property("value", &Frame<type>::m_Value) \
            .property("in", &Frame<type>::m_In) \
            .property("out", &Frame<type>::m_Out) \
            ;

#define REGISTER_CUSTOM_NAME(type, name) \
    rttr::registration::class_<Track<type>>("Track_"#name) \
            .property("frames", &Track<type>::m_vFrames) \
            ; \
    rttr::registration::class_<Frame<type>>("Frame_"#name) \
            .property("time", &Frame<type>::m_fTime) \
            .property("value", &Frame<type>::m_Value) \
            .property("in", &Frame<type>::m_In) \
            .property("out", &Frame<type>::m_Out) \
            ;


RTTR_REGISTRATION
{
    //--
    //      PRIMITIVES
    //--
    REGISTER(int)
    REGISTER(float)

    //--
    //      MATHS
    //--
    REGISTER_CUSTOM_NAME(glm::vec2, vec2)
    REGISTER_CUSTOM_NAME(glm::vec3, vec3)
    REGISTER_CUSTOM_NAME(glm::vec4, vec4)

    //--
    //      ASSET
    //--
    // REGISTER_CUSTOM_NAME(fox::Texture2D, texture)
}

#undef REGISTER
#undef REGISTER_CUSTOM_NAME