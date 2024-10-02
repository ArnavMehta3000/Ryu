#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12DescriptorHeap : public ComPtr<ID3D12DescriptorHeap>
	{
	public:
		DX12DescriptorHeap() = default;
		DX12DescriptorHeap(InterfaceType* ptr) : ComPtr(ptr) {}

		static NODISCARD CreateResult<DX12DescriptorHeap> Create(const DX12Device device, u32 descriptorCount, 
			D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	};
}
