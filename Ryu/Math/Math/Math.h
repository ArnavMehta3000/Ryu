#pragma once
#include <Windows.h>
#include <External/SimpleMath/SimpleMath.h>

namespace SM = DirectX::SimpleMath;

namespace Ryu::Math
{
	constexpr float BytesToKiloBytes = 1.0f / (1 << 10);
	constexpr float BytesToMegaBytes = 1.0f / (1 << 20);
	constexpr float BytesToGigaBytes = 1.0f / (1 << 30);

	constexpr UINT KilobytesToBytes = 1 << 10;
	constexpr UINT MegaBytesToBytes = 1 << 20;
	constexpr UINT GigaBytesToBytes = 1 << 30;
}
