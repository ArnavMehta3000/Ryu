#pragma once
#include "Common/Enum.h"

namespace Ryu::Graphics
{
	TENUM()
	enum class API
	{
		DirectX11,
		DirectX12
	};
}

#include "API.gen.h"
