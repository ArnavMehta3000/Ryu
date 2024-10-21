#include "DX12Helpers.h"
#include "Logger/Logger.h"
#include "Graphics/DX12/DX12Core.h"

namespace Ryu::Graphics::DX12::Helpers
{
	ID3D12RootSignature* CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& desc)
	{
		RYU_LOG_CATEGORY(DX12Helper);

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc;
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rsDesc.Desc_1_1 = desc;

		ComPtr<ID3DBlob> rsBlob{ nullptr };
		ComPtr<ID3DBlob> errorBlob{ nullptr };

		HRESULT hr{ S_OK };
		if (FAILED(hr = D3D12SerializeVersionedRootSignature(&rsDesc, &rsBlob, &errorBlob)))
		{
#if defined(RYU_BUILD_DEBUG)
			LOG_ERROR(RYU_USE_LOG_CATEGORY(DX12Helper), "Failed to serialize root signature. Error: {}", 
				reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
#endif
			return nullptr;
		}

		DEBUG_ASSERT(rsBlob, "Root signature blob is null");
		
		ID3D12RootSignature* rootSignature{ nullptr };
		DXCall(hr = Core::GetDevice()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

		if (FAILED(hr))
		{
			Graphics::Release(rootSignature);
		}

		return rootSignature;
	}
}
