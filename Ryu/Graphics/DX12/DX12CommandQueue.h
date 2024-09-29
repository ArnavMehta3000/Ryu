#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12CommandQueue : public ComPtr<ID3D12CommandQueue>
	{
	public:
		DX12CommandQueue() = default;
		explicit DX12CommandQueue(InterfaceType* ptr);
		~DX12CommandQueue();

		static NODISCARD CreateResult<InterfaceType*> Create(
			const DX12Device& device,
			D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void Init(const DX12Device& device);

		inline ID3D12Fence* GetFence() const noexcept { return m_fence.Get(); }
		inline u64 GetCurrentFenceValue() const noexcept { return m_currentFenceValue; }

		void Release();

	private:
		ComPtr<ID3D12Fence> m_fence;
		u64 m_currentFenceValue = 0;
	};
}
