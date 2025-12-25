#pragma once
#include <filesystem>
#include <functional>
#include <optional>

namespace Ryu::Asset
{
    namespace fs = std::filesystem;

    template<typename T>
    struct AssetLoader
    {
        using LoadFunc = std::optional<std::unique_ptr<T>>(*)(const fs::path&);
        using UnloadFunc = void(*)(T*);

        LoadFunc Load     = nullptr;
        UnloadFunc Unload = nullptr;
    };
}
