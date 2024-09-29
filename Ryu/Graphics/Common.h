#pragma once

#include <Core/Includes.h>
#include <Graphics/Internal/Log.h>

#include <Windows.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#if defined(RYU_BUILD_DEBUG)
#include <dxgidebug.h>
#endif

#include <expected>
#include <wrl/client.h>

namespace Ryu
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	using CreateResult = std::expected<T, HRESULT>;
}
