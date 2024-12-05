#pragma once
#include "Common/Enum.h"

namespace Ryu::Graphics
{
	enum class API
	{
		DirectX12 = 0,
		DirectX11
	};

	template<API API_TYPE>
	struct APITraits;
}

RYU_BEGIN_ENUM_TO_STRING(Ryu::Graphics::API)
	RYU_ENUM_TO_STRING(DirectX12)
	RYU_ENUM_TO_STRING(DirectX11)
RYU_END_ENUM_TO_STRING()
