//
// Created by samuel on 02/11/22.
//

#include "EditorIcons.hpp"
#include "Asset/TextureImporter.hpp"

namespace fox
{
    Ref<Texture2D> EditorIcons::IconPlay = nullptr;
    Ref<Texture2D> EditorIcons::IconSimulate = nullptr;
    Ref<Texture2D> EditorIcons::IconStop = nullptr;

    void EditorIcons::Init()
    {
        IconPlay = TextureImporter::LoadTexture2D("Resources/PlayButton.png");
        IconSimulate = TextureImporter::LoadTexture2D("Resources/SimulateButton.png");
        IconStop = TextureImporter::LoadTexture2D("Resources/StopButton.png");
    }
}
