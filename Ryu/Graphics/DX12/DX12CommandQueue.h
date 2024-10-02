#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12CommandQueue : public ComPtr<ID3D12CommandQueue>
	{
	public:
		DX12CommandQueue() = default;
		DX12CommandQueue(InterfaceType* ptr) : ComPtr(ptr) {}
		~DX12CommandQueue() = default;

		static NODISCARD CreateResult<DX12CommandQueue> Create(
			const DX12Device& device,
			D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void Release();
	};
}
