#pragma once
#include <directx/d3dx12.h>

namespace Ryu::Graphics::DX12
{
	using IDX12Device              = ID3D12Device8;
	using IDX12Resource            = ID3D12Resource;
	using IDX12CommandQueue        = ID3D12CommandQueue;
	using IDX12DebugCommandQueue   = ID3D12DebugCommandQueue1;
	using IDX12CommandAllocator    = ID3D12CommandAllocator;
	using IDX12CommandList         = ID3D12CommandList;
	using IDX12GraphicsCommandList = ID3D12GraphicsCommandList6;
	using IDX12DebugCommandList    = ID3D12DebugCommandList3;
	using IDX12DescriptorHeap      = ID3D12DescriptorHeap;
	using IDX12Fence               = ID3D12Fence;

	class DX12Device;

	class DeviceDependantResource
	{
		const DX12Device* m_device{ nullptr };
	};
}
