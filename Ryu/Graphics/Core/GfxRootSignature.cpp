#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxDevice.h"
#include <dxcapi.h>

namespace Ryu::Gfx
{
	RootSignature::RootSignature(Device* parent, const CD3DX12_ROOT_SIGNATURE_DESC& desc, std::string_view name)
		: DeviceChild(parent)
	{
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		if (FAILED(::D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
		{
			RYU_LOG_ERROR("Failed to serialize root signature!");
			if (error)
			{
				const u64 size = error->GetBufferSize();
				std::string_view errorMsg(reinterpret_cast<const char*>(error->GetBufferPointer()), size);
				RYU_LOG_ERROR("{}", errorMsg.data());
			}
			return;
		}

		CreateInternal(signature->GetBufferPointer(), signature->GetBufferSize(), name);
	}

	RootSignature::RootSignature(Device* parent, IDxcBlob* blob, std::string_view name)
		: DeviceChild(parent)
	{
		CreateInternal(blob->GetBufferPointer(), blob->GetBufferSize(), name);
	}

	void RootSignature::CreateInternal(void* data, u64 size, std::string_view name)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		DXCall(device->CreateRootSignature(0, data, size, IID_PPV_ARGS(&m_rootSignature)));
		DX12::SetObjectName(m_rootSignature.Get(), name.data());
	}
}
