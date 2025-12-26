#pragma once
#include <functional>

namespace Ryu::Asset
{
	using AssetId = u64;
	constexpr AssetId INVALID_ASSET_ID = 0;

	enum class AssetState : u8
    {
        Unloaded,   // Registered but CPU data not loaded
        Loading,    // Currently loading CPU data
        Loaded,     // CPU data ready, GPU resource not created
        Ready,      // GPU resource created and ready
        Failed      // Load failed
    };

	template<typename T>
	struct AssetHandle
	{
		AssetId id = INVALID_ASSET_ID;

		constexpr AssetHandle() = default;
		constexpr explicit AssetHandle(AssetId id) : id(id) {}

		// For use with entt::hashed_string
		constexpr explicit AssetHandle(const char* str)
			: id(std::hash<std::string_view>{}(str))
		{
		}

		[[nodiscard]] constexpr bool IsValid() const { return id != INVALID_ASSET_ID; }
		constexpr explicit operator bool() const { return IsValid(); }
		constexpr bool operator==(const AssetHandle&) const = default;
		constexpr auto operator<=>(const AssetHandle&) const = default;
	};
}

template<typename T>
struct std::hash<Ryu::Asset::AssetHandle<T>>
{
	size_t operator()(const Ryu::Asset::AssetHandle<T>& h) const noexcept
	{
		return std::hash<Ryu::Asset::AssetId>{}(h.id);
	}
};
