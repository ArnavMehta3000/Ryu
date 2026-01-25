#pragma once

namespace Ryu::Graphics
{
	enum class API
	{
		DirectX11,
		DirectX12,
		Vulkan
	};
}

RYU_ENUM_TO_STRING(Ryu::Graphics::API,
{
	switch (value)
	{
		using enum Ryu::Graphics::API;
		case DirectX11: return "DirectX11";
		case DirectX12: return "DirectX12";
		case Vulkan:    return "Vulkan";
		default:        return "<Unknown>";
	};
})
