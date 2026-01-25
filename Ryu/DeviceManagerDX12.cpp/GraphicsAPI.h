#pragma once

namespace Ryu::Graphics
{
	enum class GraphicsAPI
	{
		DirectX11,
		DirectX12,
		Vulkan
	};
}

RYU_ENUM_TO_STRING(Ryu::Graphics::GraphicsAPI,
{
	switch (value)
	{
		using enum Ryu::Graphics::GraphicsAPI;
	case DirectX11: return "DirectX11";
	case DirectX12: return "DirectX12";
	case Vulkan:    return "Vulkan";
	default:        return "<Unknown>";
	};
})


