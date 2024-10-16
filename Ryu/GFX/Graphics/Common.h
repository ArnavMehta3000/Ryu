#pragma once
#include "Common/Common.h"
#include "Logger/Logger.h"
#include "directx/d3dx12.h"
#include <Windows.h>
#include <dxgidebug.h>
#include <wrl/client.h>
#include <string>

namespace Ryu::Graphics
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	void SetName(ID3D12Object* obj, std::wstring name)
	{
		obj->SetName(name.c_str());
	}

	// Log category only for graphics panic (hresult fail)
	LOG_CATEGORY(GraphicsPanic);

	namespace Internal
	{
		void LogHResultError(HRESULT hr, std::string_view functionName, std::string_view fileName, u32 lineNumber);
	}
}

// Assert on HRESULT
#define HR(hr)\
{\
	if (FAILED(hr))\
	{\
		Ryu::Graphics::Internal::LogHResultError(hr, __FUNCTION__, __FILE__, __LINE__); \
	}\
}
