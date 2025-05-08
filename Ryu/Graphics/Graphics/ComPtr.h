#pragma once
#include <wrl/client.h>

namespace Ryu::Gfx
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}
