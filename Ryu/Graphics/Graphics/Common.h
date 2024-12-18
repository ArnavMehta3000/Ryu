#pragma once
#include "Common/Common.h"
#include "Logger/Logger.h"
#include <directx/d3dx12.h>
#include <d3d11_4.h>
#include <d3d11shader.h>
#include <Windows.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl/client.h>
#include <string>

namespace Ryu::Graphics
{
	constexpr u32 FRAME_BUFFER_COUNT{ 3 };

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
	RYU_LOG_CATEGORY(GraphicsPanic);

	// Log category only for graphics debugging
	RYU_LOG_CATEGORY(GraphicsDebug);


	namespace Internal
	{
		void LogHResultError(HRESULT hr, std::string_view functionName, std::string_view fileName, u32 lineNumber);
		void SetNameDX12(ID3D12Object* obj, std::wstring name);
		
		void LogDX11Naming(std::string_view name);
		
		template <typename T>
		inline void SetDebugObjectName(_In_ T resource, _In_z_ std::string_view name)
		{
			if (resource)
			{
				resource->SetPrivateData(WKPDID_D3DDebugObjectName, (u32)name.length(), name.data());
				LogDX11Naming(name);
			}
		}
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

#if defined(RYU_BUILD_DEBUG)
#define DX11_NAME_OBJECT(obj, name) Ryu::Graphics::Internal::SetDebugObjectName(obj, name);
#else
#define DX11_NAME_OBJECT(obj, name)
#endif

