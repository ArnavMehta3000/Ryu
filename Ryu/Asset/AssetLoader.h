#pragma once
#include "Asset/AssetData.h"
#include <filesystem>

namespace Ryu::Asset
{
	namespace fs = std::filesystem;

    template<typename T>
    std::unique_ptr<T> LoadAsset(const std::filesystem::path& path);

    template<> std::unique_ptr<MeshData> LoadAsset(const std::filesystem::path& path);
    template<> std::unique_ptr<TextureData> LoadAsset(const std::filesystem::path& path);
}
