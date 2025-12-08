#pragma once
#include <filesystem>
#include <memory>
#include <functional>

namespace Ryu::Asset
{
    namespace fs = std::filesystem;

    template<typename T>
    struct AssetLoader
    {
        using LoadFunc = std::unique_ptr<T>(*)(const fs::path&);
        using UnloadFunc = void(*)(T*);

        LoadFunc Load     = nullptr;
        UnloadFunc Unload = nullptr;
    };
}
