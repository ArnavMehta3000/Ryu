#pragma once
#include <wrl/client.h>

namespace Ryu::Graphics
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	constexpr void Release(T*& ptr)
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
}
