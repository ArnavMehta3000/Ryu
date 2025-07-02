#include "Graphics/Core/DX12.h"
#include "Common/StandardTypes.h"
#include "Utils/StringConv.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include <numeric>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(D3D12);

	void DX12::SetObjectName(DX12::Object* object, const char* name)
	{
		if (object)
		{
			DXCall(object->SetPrivateData(WKPDID_D3DDebugObjectName, (u32)strlen(name) + 1, name));
			RYU_LOG_TRACE(LogD3D12, "Set D3D12 object name: {}", name);
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

	D3D12_COMMAND_LIST_TYPE DX12::ToNative(CommandListType type)
	{
		using enum Ryu::Gfx::CommandListType;
		
		switch (type)
		{
		case None:         return D3D12_COMMAND_LIST_TYPE_NONE;
		case Direct:       return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case Bundle:       return D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case Compute:      return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case Copy:         return D3D12_COMMAND_LIST_TYPE_COPY;
		case VideoDecode:  return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
		case VideoProcess: return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
		case VideoEncode:  return D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;
		default:           return D3D12_COMMAND_LIST_TYPE_NONE;
		}
	}

	D3D12_COMMAND_QUEUE_PRIORITY DX12::ToNative(CommandQueuePriority priority)
	{
		using enum Ryu::Gfx::CommandQueuePriority;

		switch (priority)
		{
		case Normal:   return D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		case High:     return D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		case Realtime: return D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME;
		default:       return D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		}
	}

	D3D12_DESCRIPTOR_HEAP_TYPE DX12::ToNative(DescriptorHeapType type)
	{
		using enum Ryu::Gfx::DescriptorHeapType;

		switch (type)
		{
		case CBV_SRV_UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case Sampler:     return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case RTV:         return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case DSV:         return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		default:          return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		}
	}

	D3D12_FENCE_FLAGS DX12::ToNative(FenceFlag flag)
	{
		using enum Ryu::Gfx::FenceFlag;

		switch (flag)
		{
		case None:               return D3D12_FENCE_FLAG_NONE;
		case Shared:             return D3D12_FENCE_FLAG_SHARED;
		case SharedCrossAdapter: return D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER;
		case Monitored:          return D3D12_FENCE_FLAG_NON_MONITORED;
		default:                 return D3D12_FENCE_FLAG_NONE;
		}
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
		MAYBE_UNUSED HRESULT hr,
		MAYBE_UNUSED DX12::Device* device,
		MAYBE_UNUSED const char* code,
		MAYBE_UNUSED const char* fileName,
		MAYBE_UNUSED const char* functionName,
		MAYBE_UNUSED u32 lineNumber)
	{
		if (FAILED(hr))
		{
#if defined(RYU_THROW_ON_FAIL_HRESULT)
			RYU_LOG_FATAL(LogD3D12, "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
#else
			RYU_LOG_ERROR(LogD3D12, "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, Internal::GetErrorString(hr, device));
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

	FreeList::FreeList(u32 size)
		: m_numAllocations(0)
	{
		m_freeList.resize(size);
		std::iota(m_freeList.begin(), m_freeList.end(), 0);
	}

	FreeList::~FreeList()
	{
		RYU_ASSERT(m_numAllocations == 0, "Free list not fully released");
	}

	u32 FreeList::Allocate()
	{
		const u32 slot = m_numAllocations++;
		RYU_ASSERT(slot < (u32)m_freeList.size());
		return m_freeList[slot];
	}

	void FreeList::Free(u32 index)
	{
		u32 freeIndex = m_numAllocations--;
		RYU_ASSERT(freeIndex > 0);

		m_freeList[freeIndex] = index;
	}

	bool FreeList::CanAllocate() const noexcept 
	{
		return m_numAllocations < static_cast<u32>(m_freeList.size());
	}

	DXGI_FORMAT DXGI::ToNative(Format format)
	{
		return g_DXGIFormatMap[(u32)format];
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
}
