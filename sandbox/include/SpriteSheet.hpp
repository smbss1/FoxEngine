//
// Created by samuel on 28/06/2021.
//

#ifndef FOXENGINE_SPRITE_SHEET_HPP
#define FOXENGINE_SPRITE_SHEET_HPP

#include "FoxEngine.hpp"

class SpriteSheetState : public fox::State
{
public:
    SpriteSheetState()
            : m_Camera(1280.f / 720.f, true)
            , fox::State("Example2D") { }

    ~SpriteSheetState() override = default;
    void OnEnter() override;
    void OnExit() override;
    void OnEvent(fox::Event& event) override;
    void OnUpdate() override;
    void OnImGui() override;

private:
    fox::OrthographicCameraController m_Camera;

    fox::ref<fox::Texture2D> m_pSpriteSheet;
    fox::ref<fox::SubTexture2D> m_TextureStairs;
    fox::ref<fox::SubTexture2D> m_TextureBarrel;
    fox::ref<fox::SubTexture2D> m_TextureTree;
};


#endif //FOXENGINE_SPRITE_SHEET_HPP
