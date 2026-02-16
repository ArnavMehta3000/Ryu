#include "Asset/AssetLoader.h"
#include "Asset/Loaders/OBJLoader.h"
#include "Asset/Loaders/ImageLoader.h"

namespace Ryu::Asset
{
    template<>
    std::unique_ptr<MeshData> LoadAsset(const fs::path& path)
    {
        const auto ext = path.extension().string();
        if (ext == ".obj") return OBJLoader::Load(path);
        // if (ext == ".gltf") return GLTFLoader::Load(path);
        return nullptr;
    }

    template<>
    std::unique_ptr<TextureData> LoadAsset(const fs::path& path)
    {
        const auto ext = path.extension().string();
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
        {
            return ImageLoader::Load(path);
        }
        return nullptr;
    }
}
