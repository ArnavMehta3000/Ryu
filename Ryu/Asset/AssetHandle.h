#pragma once
#include "Common/StandardTypes.h"

namespace Ryu::Asset
{
    enum class AssetState
    {
        Unloaded,  // Asset registered but not yet loaded
        Loading,   // Asset is currently being loaded
        Ready,     // Asset loaded and ready to use
        Failed     // Asset failed to load
    };

    template<typename T>
    struct AssetHandle
    {
        static constexpr u64 INVALID_ID = 0;

        u64 Id = 0;
        u32 Generation = 0;

        bool IsValid() const { return Id != INVALID_ID; }
        operator bool() const { return IsValid(); }
        bool operator==(const AssetHandle&) const = default;
    };
}
