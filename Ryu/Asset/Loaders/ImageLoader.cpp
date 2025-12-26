#include "Asset/Loaders/ImageLoader.h"
#include "Asset/AssetData.h"
#include "Core/Logging/Logger.h"
#include <STB/stb_image.h>

namespace Ryu::Asset
{
    std::optional<std::unique_ptr<TextureData>> ImageLoader::Load(const fs::path& path)
    {
        i32 width = 0, height = 0, channels = 0;
        ::stbi_set_flip_vertically_on_load(true);

        RYU_LOG_TRACE("Trying to load image {}", path.string());

        if (byte* pixels = ::stbi_load(path.string().c_str(), &width, &height, &channels, 0))
        {
            u64 dataSize = static_cast<u64>(width * height * channels);

            auto texture = std::make_unique<TextureData>();
            texture->Width = width;
            texture->Height = height;
            texture->Data.assign(pixels, pixels + dataSize);

            // Map channels to format
            switch (channels)
            {
                case 1: texture->PixelFormat = TextureData::Format::R8; break;
                case 2: texture->PixelFormat = TextureData::Format::RG8; break;
                case 3: texture->PixelFormat = TextureData::Format::RGB8; break;
                case 4: texture->PixelFormat = TextureData::Format::RGBA8; break;
            }

            ::stbi_image_free(pixels);
            return texture;
        }

        RYU_LOG_WARN("Failed to load image {}", path.string());
        return std::nullopt;
    }
}
