#pragma once
#include "Common/StandardTypes.h"

namespace Ryu::Asset
{
    enum class AssetState
    {
        Loading,
        Ready,
        Failed
    };

    template<typename T>
    struct AssetHandle
    {
        static constexpr u64 INVALID_ID = 0;

        u64 Id = 0;
        u32 Generation = 0;

        bool IsValid() const { return Id != INVALID_ID; }
        bool operator==(const AssetHandle&) const = default;
    };
}
