#pragma once
#include "Logger/Logger.h"
#include <d3dcommon.h>
#include <string_view>
#include <string>

struct ID3D12Object;

namespace Ryu::Graphics::Internal
{
	// Log category only for graphics panic (HRESULT fail)
	RYU_LOG_DECLARE_CATEGORY(GraphicsPanic);

	// Log category only for graphics debugging
	RYU_LOG_DECLARE_CATEGORY(GraphicsDebug);

	void LogHResultError(long hr, std::string_view functionName, std::string_view fileName, unsigned int lineNumber);

	void SetNameDX12(ID3D12Object* obj, std::wstring name);

	void LogDX11Naming(const std::string& name);

	template <typename T>
	inline void SetDebugObjectName(_In_ T resource, _In_z_ std::string_view name)
	{
		if (resource)
		{
			// Buffer to store the current name
			char currentName[256];
			unsigned int nameLength = sizeof(currentName);

			HRESULT result = resource->GetPrivateData(WKPDID_D3DDebugObjectName, &nameLength, currentName);
			std::string nameStr(name);

			// Check if there was an existing name
			if (SUCCEEDED(result) && nameLength > 0)
			{
				std::string oldName(currentName, nameLength);
				if (oldName != name) // Only log if the name is different
				{
					LogDX11Naming("DX11 rename object: \"" + oldName + "\" -> \"" + nameStr);
				}
			}
			else
			{
				LogDX11Naming("DX11 name object: " + nameStr);
			}

			// Set the new name
			resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(name.length()), name.data());
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
#define DX12_NAME_OBJECT(obj, name) ::Ryu::Graphics::Internal::SetNameDX12(obj, name)
#else
#define DX12_NAME_OBJECT(obj, name)
#endif

#if defined(RYU_BUILD_DEBUG)
#define DX11_NAME_OBJECT(obj, name) ::Ryu::Graphics::Internal::SetDebugObjectName(obj, name)
#else
#define DX11_NAME_OBJECT(obj, name)
#endif
