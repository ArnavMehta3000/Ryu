#pragma once
#include "Asset/AssetLoader.h"

namespace Ryu::Asset
{
    struct MeshData;

    class OBJLoader
    {
    public:
        static std::optional<std::unique_ptr<MeshData>> Load(const fs::path& path);
    };
}
