#include "Graphics/Core/GfxRootSignature.h"
#include "Graphics/Core/GfxDevice.h"

namespace Ryu::Gfx
{
	RootSignature::RootSignature(Device* parent, const CD3DX12_ROOT_SIGNATURE_DESC& desc, std::string_view name)
		: DeviceChild(parent)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

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

		DXCall(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		DX12::SetObjectName(m_rootSignature.Get(), name.data());
	}
}
