#pragma once
#include "Common/Common.h"
#include "Logger/Logger.h"
#include <directx/d3dx12.h>
#include <Windows.h>
#include <dxgi1_6.h>
#if defined(RYU_BUILD_DEBUG)
#include <dxgidebug.h>
#endif
#include <wrl/client.h>
#include <string>

namespace Ryu::Graphics
{
	template <typename T>
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

	// Log category only for graphics panic (hresult fail)
	LOG_CATEGORY(GraphicsPanic);

	// Log category only for graphics debugging
	LOG_CATEGORY(GraphicsDebug);


	namespace Internal
	{
		void LogHResultError(HRESULT hr, std::string_view functionName, std::string_view fileName, u32 lineNumber);
		void SetNameDX12(ID3D12Object* obj, std::wstring name);
	}
}

// Assert on HRESULT
#define DXCall(hr)\
{\
	if (FAILED(hr))\
	{\
		Ryu::Graphics::Internal::LogHResultError(hr, __FUNCTION__, __FILE__, __LINE__); \
	}\
}

#if defined(RYU_BUILD_DEBUG)
#define DX12_NAME_OBJECT(obj, name) Ryu::Graphics::Internal::SetNameDX12(obj, name);
#else
#define DX12_NAME_OBJECT(obj, name)
#endif
