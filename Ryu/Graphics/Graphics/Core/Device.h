#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		static DevicePtr Create();
		static void Destroy(Device& device);

		CommandList CreateCommandList(CommandAllocator& allocator, CommandListType type) const;
		CommandQueue CreateCommandQueue(CommandListType type, CommandQueuePriority priority) const;
		CommandAllocator CreateCommandAllocator(CommandListType type) const;
		Fence CreateFence(u64 initialValue = 0, FenceFlag flag = FenceFlag::None) const;

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD DX12::CommandQueue* const GetCommandQueue() const noexcept { return m_cmdQueue.Get(); }
		inline NODISCARD DX12::GraphicsCommandList* const GetCommandList() const noexcept { return m_cmdList.Get(); }
		inline NODISCARD DX12::CommandAllocator* const GetCommandAllocator(const u32 frameIndex) const noexcept { return m_cmdAllocators[frameIndex].Get(); }
		inline NODISCARD DX12::Fence* const GetFence() const noexcept { return m_fence.Get(); }
		inline NODISCARD FrameArray<u64>& GetFenceValues() noexcept { return m_fenceValues; }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD DescriptorHeap& GetRTVDescriptorHeap() noexcept { return m_rtvHeap; }

	private:
		void Initialize();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateCommandList();
		void CreateSynchronization();
		void CreateDescriptorHeap();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	public:
		void WaitForGPU();
		void MoveToNextFrame(const u32 frameIndex);

	private:
		CD3DX12FeatureSupport                      m_featureSupport;
		HANDLE                                     m_fenceEvent{ nullptr };
		FrameArray<u64>                            m_fenceValues{0};
		u32                                        m_frameIndex = 0;
		ComPtr<DX12::Fence>                        m_fence;
		ComPtr<DXGI::Factory>                      m_factory;
		ComPtr<DX12::Device>                       m_device;
		ComPtr<DX12::CommandQueue>                 m_cmdQueue;
		FrameArray<ComPtr<DX12::CommandAllocator>> m_cmdAllocators;
		ComPtr<DX12::GraphicsCommandList>          m_cmdList;
		DescriptorHeap                             m_rtvHeap;
	};
}
