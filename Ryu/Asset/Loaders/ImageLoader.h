#pragma once
#include "Asset/AssetLoader.h"

namespace Ryu::Asset
{
    struct TextureData;

    class ImageLoader
    {
    public:
        static std::unique_ptr<TextureData> Load(const fs::path& path);
    };

    using JPGLoader = ImageLoader;
    using PNGLoader = ImageLoader;
    using BMPLoader = ImageLoader;
    using HDRLoader = ImageLoader;
}
