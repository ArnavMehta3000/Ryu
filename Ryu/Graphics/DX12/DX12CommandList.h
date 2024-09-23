#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12CommandList : public ComPtr<ID3D12CommandList>
	{
	public:
		DX12CommandList() = default;
		~DX12CommandList();

		void Create(const DX12Device& device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void Release();

	private:
		ComPtr<ID3D12CommandAllocator> m_allocator;
	};
}
