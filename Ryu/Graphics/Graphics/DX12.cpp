#include "Graphics/DX12.h"
#include "Common/StandardTypes.h"
#include "Utils/StringConv.h"
#include "Logger/Logger.h"

namespace Ryu::Gfx
{
	void DX12::SetObjectName(DX12::Object* object, const char* name)
	{
		if (object)
		{
			DXCall(object->SetPrivateData(WKPDID_D3DDebugObjectName, (u32)strlen(name) + 1, name));
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


	constexpr std::string_view Internal::CommandListTypeToString(D3D12_COMMAND_LIST_TYPE type)
	{
#define STATE_CASE(name) case D3D12_COMMAND_LIST_TYPE_##name: return #name
		switch (type)
		{
			STATE_CASE(DIRECT);
			STATE_CASE(COMPUTE);
			STATE_CASE(COPY);
			STATE_CASE(BUNDLE);
			STATE_CASE(VIDEO_DECODE);
			STATE_CASE(VIDEO_ENCODE);
			STATE_CASE(VIDEO_PROCESS);
		default: return "<UNKNOWN>";
		}
#undef STATE_CASE
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
		MAYBE_UNUSED HRESULT hr,
		MAYBE_UNUSED DX12::Device* device,
		MAYBE_UNUSED const char* code,
		MAYBE_UNUSED const char* fileName,
		MAYBE_UNUSED const char* functionName,
		MAYBE_UNUSED u32 lineNumber)
	{
		if (FAILED(hr))
		{
#if defined(RYU_BREAK_ON_FAIL_HRESULT)
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(DX12), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
#else
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(DX12), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
#endif
			return false;
		}

		return true;
	}

	constexpr static const DXGI_FORMAT g_DXGIFormatMap[] =
	{
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8_UINT,
		DXGI_FORMAT_R8_SINT,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_SNORM,
		DXGI_FORMAT_R8G8_UINT,
		DXGI_FORMAT_R8G8_SINT,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_R8G8_SNORM,
		DXGI_FORMAT_R16_UINT,
		DXGI_FORMAT_R16_SINT,
		DXGI_FORMAT_R16_UNORM,
		DXGI_FORMAT_R16_SNORM,
		DXGI_FORMAT_R16_FLOAT,
		DXGI_FORMAT_B4G4R4A4_UNORM,
		DXGI_FORMAT_B5G6R5_UNORM,
		DXGI_FORMAT_B5G5R5A1_UNORM,
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R8G8B8A8_SINT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_SNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_R10G10B10A2_UNORM,
		DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_R16G16_UINT,
		DXGI_FORMAT_R16G16_SINT,
		DXGI_FORMAT_R16G16_UNORM,
		DXGI_FORMAT_R16G16_SNORM,
		DXGI_FORMAT_R16G16_FLOAT,
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_UINT,
		DXGI_FORMAT_R16G16B16A16_SINT,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_UNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_UINT,
		DXGI_FORMAT_R32G32B32A32_SINT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,

		DXGI_FORMAT_R9G9B9E5_SHAREDEXP,

		DXGI_FORMAT_BC1_UNORM,
		DXGI_FORMAT_BC2_UNORM,
		DXGI_FORMAT_BC3_UNORM,
		DXGI_FORMAT_BC4_UNORM,
		DXGI_FORMAT_BC4_SNORM,
		DXGI_FORMAT_BC5_UNORM,
		DXGI_FORMAT_BC5_SNORM,
		DXGI_FORMAT_BC6H_UF16,
		DXGI_FORMAT_BC6H_SF16,
		DXGI_FORMAT_BC7_UNORM,

		DXGI_FORMAT_D16_UNORM,
		DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	};
	
	static_assert(ARRAYSIZE(g_DXGIFormatMap) == (u32)::Ryu::Gfx::Format::_COUNT);
}

constexpr DXGI_FORMAT Ryu::Gfx::DXGI::ConvertFormat(Format format)
{
	return g_DXGIFormatMap[(u32)format];
}

constexpr DXGI_FORMAT Ryu::Gfx::DXGI::GetFormatSRGB(DXGI_FORMAT format, bool srgb)
{
	if (!srgb)
	{
		return format;
	}

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
