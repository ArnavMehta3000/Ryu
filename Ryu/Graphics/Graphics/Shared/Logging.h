#pragma once
#include "Logger/Logger.h"
#include <d3dcommon.h>
#include <string_view>
#include <string>

struct ID3D12Object;

namespace Ryu::Graphics::Internal
{
	// Log category only for graphics panic (HRESULT fail)
	RYU_LOG_CATEGORY(GraphicsPanic);

	// Log category only for graphics debugging
	RYU_LOG_CATEGORY(GraphicsDebug);

	void LogHResultError(long hr, std::string_view functionName, std::string_view fileName, unsigned int lineNumber);

	void SetNameDX12(ID3D12Object* obj, std::wstring name);

	void LogDX11Naming(std::string_view name);

	template <typename T>
	inline void SetDebugObjectName(_In_ T resource, _In_z_ std::string_view name)
	{
		if (resource)
		{
			resource->SetPrivateData(WKPDID_D3DDebugObjectName, (unsigned int)name.length(), name.data());
			LogDX11Naming(name);
		}
	}
}

// Assert on HRESULT
#define DXCall(hr)\
{\
	if (FAILED(hr))\
	{\
		::Ryu::Graphics::Internal::LogHResultError(hr, __FUNCTION__, __FILE__, __LINE__); \
	}\
}

#if defined(RYU_BUILD_DEBUG)
#define DX12_NAME_OBJECT(obj, name) ::Ryu::Graphics::Internal::SetNameDX12(obj, name);
#else
#define DX12_NAME_OBJECT(obj, name)
#endif

#if defined(RYU_BUILD_DEBUG)
#define DX11_NAME_OBJECT(obj, name) ::Ryu::Graphics::Internal::SetDebugObjectName(obj, name);
#else
#define DX11_NAME_OBJECT(obj, name)
#endif
