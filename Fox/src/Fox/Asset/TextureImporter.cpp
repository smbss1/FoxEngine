#include "TextureImporter.hpp"

#include "Core/Project.hpp"
#include "Core/Buffer.hpp"

#include <stb_image.h>

namespace fox
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		//FOX_PROFILE_FUNCTION();

		return LoadTexture2D(Project::AssetsDir() / metadata.FilePath);
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		//FOX_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;

		{
			//FOX_PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
			std::string pathStr = path.string();
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		if (data.Data == nullptr)
		{
			FOX_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		// TODO: think about this
		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();
		return texture;
	}

}