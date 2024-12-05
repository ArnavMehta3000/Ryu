#pragma once
#include "Graphics/API.h"
#include <directx/d3dx12.h>

namespace Ryu::Graphics
{
	namespace DX12
	{
		using IDX12Device              = ID3D12Device8;
		using IDX12Resource            = ID3D12Resource;
		using IDX12CommandQueue        = ID3D12CommandQueue;
		using IDX12CommandAllocator    = ID3D12CommandAllocator;
		using IDX12CommandList         = ID3D12GraphicsCommandList8;
		using IDX12GraphicsCommandList = ID3D12GraphicsCommandList6;
		using IDX12DescriptorHeap      = ID3D12DescriptorHeap;
		using IDX12Fence               = ID3D12Fence1;
	}

	template<>
	struct APITraits<API::DirectX12>
	{
		using DeviceType              = DX12::IDX12Device;
		using ResourceType            = DX12::IDX12Resource;
		using CommandQueueType        = DX12::IDX12CommandQueue;
		using CommandAllocatorType    = DX12::IDX12CommandAllocator;
		using CommandListType         = DX12::IDX12CommandList;
		using GraphicsCommandListType = DX12::IDX12GraphicsCommandList;
		using DescriptorHeapType      = DX12::IDX12DescriptorHeap;
		using FenceType               = DX12::IDX12Fence;
	};
}
