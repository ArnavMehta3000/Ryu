#pragma once
#include <Graphics/DX12/DX12CommandAllocator.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12GraphicsCommandList : public ComPtr<ID3D12GraphicsCommandList>
	{
	public:
		DX12GraphicsCommandList() = default;
		DX12GraphicsCommandList(InterfaceType* ptr) : ComPtr(ptr) {}
		~DX12GraphicsCommandList() = default;

		static CreateResult<DX12GraphicsCommandList> Create(const DX12Device& device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		const DX12CommandAllocator& GetAllocator() const { return m_allocator; }
		void Release();
		void ResetCommandList();

	private:
		DX12CommandAllocator m_allocator;
	};
}
