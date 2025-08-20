#pragma once
#include <Windows.h>
#include <External/SimpleMath/SimpleMath.h>

namespace SM = DirectX::SimpleMath;

namespace Ryu::Math
{
	constexpr auto BytesToKB(auto bytes) { return bytes / 1024.0f; }
	constexpr auto BytesToMB(auto bytes) { return BytesToKB(bytes) / 1024.0f; }
	constexpr auto BytesToGB(auto bytes) { return BytesToMB(bytes) / 1024.0f; }
}
