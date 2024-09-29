#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12CommandList : public ComPtr<ID3D12CommandList>
	{
	public:
		DX12CommandList() = default;
		explicit DX12CommandList(InterfaceType* ptr);
		~DX12CommandList();

		static CreateResult<InterfaceType*> Create(const DX12Device& device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void Init(const DX12Device& device);
		void ResetCommandList();
		void Release();

		inline ID3D12GraphicsCommandList* GetGFXCmdList() const noexcept { return (ID3D12GraphicsCommandList*)Get(); }

	private:
		ComPtr<ID3D12CommandAllocator> m_allocator;
	};
}
