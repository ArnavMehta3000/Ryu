#pragma once
#include "Common/Enum.h"

namespace Ryu::Graphics
{
	enum class API
	{
		Undefined,
		DirectX11,
		DirectX12
	};
}

RYU_ENUM_TO_STRING(Ryu::Graphics::API,
{
	switch (value)
	{
		using enum Ryu::Graphics::API;
		case DirectX11: return "DirectX11";
		case DirectX12: return "DirectX12";
		case Undefined:
		default: return "<Unknown API>";
	}
})
