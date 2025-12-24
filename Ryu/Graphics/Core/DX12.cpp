#include "Graphics/Core/DX12.h"
#include "Core/Common/Assert.h"
#include "Core/Common/StandardTypes.h"
#include "Core/Utils/StringConv.h"
#include <numeric>

namespace Ryu::Gfx
{
	void DX12::SetObjectName(DX12::Object* object, const char* name)
	{
		if (object && name)
		{
			DXCall(object->SetPrivateData(WKPDID_D3DDebugObjectName, (u32)strlen(name) + 1, name));
			RYU_LOG_TRACE("Set D3D12 object name: {}", name);
		}
	}

	std::string DX12::GetObjectName(DX12::Object* object)
	{
		std::string out;
		if (object)
		{
			u32 size = 0;
			if (SUCCEEDED(object->GetPrivateData(WKPDID_D3DDebugObjectName, &size, nullptr)))
			{
				out.resize(size);
				DXCall(object->GetPrivateData(WKPDID_D3DDebugObjectName, &size, &out[0]));
			}
		}
		return out;
	}

	std::string_view Internal::FeatureLevelToString(D3D_FEATURE_LEVEL level)
	{
		switch(level)
		{
			case D3D_FEATURE_LEVEL_12_2:        return "12.2";
			case D3D_FEATURE_LEVEL_12_1:        return "12.1";
			case D3D_FEATURE_LEVEL_12_0:        return "12.0";
			case D3D_FEATURE_LEVEL_11_1:        return "11.1";
			case D3D_FEATURE_LEVEL_11_0:        return "11.0";
			case D3D_FEATURE_LEVEL_10_1:        return "10.1";
			case D3D_FEATURE_LEVEL_10_0:        return "10.0";
			case D3D_FEATURE_LEVEL_9_3:         return "9.3";
			case D3D_FEATURE_LEVEL_9_2:         return "9.2";
			case D3D_FEATURE_LEVEL_9_1:         return "9.1";
			case D3D_FEATURE_LEVEL_1_0_CORE :   return "1.0 Core";
			case D3D_FEATURE_LEVEL_1_0_GENERIC: return "1.0 Generic";
			default:                            return "<UNKNOWN>";
		}
	}
	
	std::string Internal::GetErrorString(HRESULT errorCode, DX12::Device* device)
	{
		std::string str = ::Ryu::Utils::HRToString(errorCode);
		if (errorCode == DXGI_ERROR_DEVICE_REMOVED && device)
		{
			ComPtr<DX12::InfoQueue> infoQueue;
			device->QueryInterface(infoQueue.GetAddressOf());
			
			if (infoQueue)
			{
				str += "Validation Layer: \n";
				for (u64 i = 0; i < infoQueue->GetNumStoredMessages(); ++i)
				{
					// Get message length
					size_t messageLength = 0;
					infoQueue->GetMessage(0, nullptr, &messageLength);

					D3D12_MESSAGE* message = (D3D12_MESSAGE*)malloc(messageLength);
					infoQueue->GetMessage(0, message, &messageLength);

					if (message)
					{
						str += message->pDescription;
						str += "\n";
					}
					free(message);
				}
			}

			HRESULT removedReason = device->GetDeviceRemovedReason();
			str += "\nDRED: " + GetErrorString(removedReason, nullptr);
		}

		return str;
	}
	
	bool Internal::LogHRESULT(
		[[maybe_unused]] HRESULT hr,
		[[maybe_unused]] DX12::Device* device,
		[[maybe_unused]] const char* code,
		[[maybe_unused]] const char* fileName,
		[[maybe_unused]] const char* functionName,
		[[maybe_unused]] u32 lineNumber)
	{
		if (FAILED(hr))
		{
#if defined(RYU_THROW_ON_FAIL_HRESULT)
			RYU_LOG_FATAL("HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
#else
			RYU_LOG_ERROR("HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
#endif
			return false;
		}

		return true;
	}

	DXGI_FORMAT DXGI::GetFormatSRGB(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_BC1_UNORM:		 return DXGI_FORMAT_BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_UNORM:		 return DXGI_FORMAT_BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_UNORM:		 return DXGI_FORMAT_BC3_UNORM_SRGB;
		case DXGI_FORMAT_BC7_UNORM:		 return DXGI_FORMAT_BC7_UNORM_SRGB;
		default:						 return format;
		};
	}
	
	DXGI_FORMAT DXGI::GetFormatNonSRGB(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_BC1_UNORM_SRGB:      return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:      return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:      return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:      return DXGI_FORMAT_BC7_UNORM;
		default:						      return format;
		};
	}
}
