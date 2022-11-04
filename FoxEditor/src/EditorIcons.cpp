//
// Created by samuel on 02/11/22.
//

#include "EditorIcons.hpp"

namespace fox
{
    Ref<Texture2D> EditorIcons::IconPlay = nullptr;
    Ref<Texture2D> EditorIcons::IconSimulate = nullptr;
    Ref<Texture2D> EditorIcons::IconStop = nullptr;

    void EditorIcons::Init()
    {
        IconPlay = Texture2D::Create("Resources/PlayButton.png");
        IconSimulate = Texture2D::Create("Resources/SimulateButton.png");
        IconStop = Texture2D::Create("Resources/StopButton.png");
    }
}
