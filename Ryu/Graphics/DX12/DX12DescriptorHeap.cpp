#include <Graphics/DX12/DX12DescriptorHeap.h>
#include <Graphics/DX12/DX12Device.h>

namespace Ryu::Graphics
{
	CreateResult<DX12DescriptorHeap> DX12DescriptorHeap::Create(const DX12Device device, u32 descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.NumDescriptors = descriptorCount;
		desc.Type           = type;
		desc.Flags          = flags;
		desc.NodeMask       = 0;

		DX12DescriptorHeap outDescHeap;

		HRESULT hr{ S_OK };
		if (FAILED(hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(outDescHeap.ReleaseAndGetAddressOf()))))
		{
			return std::unexpected(hr);
		}

		return outDescHeap;
	}
}
