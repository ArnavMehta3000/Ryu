#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/Fence.h"
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		static std::shared_ptr<Device> Create();
		static void Destroy(Device& device);

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD DX12::CommandQueue* const GetCommandQueue() const noexcept { return m_cmdQueue.Get(); }
		inline NODISCARD DX12::GraphicsCommandList* const GetCommandList() const noexcept { return m_cmdList.Get(); }
		inline NODISCARD DX12::CommandAllocator* const GetCommandAllocator(const u32 frameIndex) const noexcept { return m_cmdAllocators[frameIndex].Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD const Fence& GetFence(const u32 frameIndex) const noexcept { return m_fences[frameIndex]; }
		inline NODISCARD DescriptorHeap& GetRTVDescriptorHeap() noexcept { return m_rtvHeap; }

	private:
		void Initialize();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateCommandList();
		void CreateSynchronization();
		void CreateDescriptorHeap();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		CD3DX12FeatureSupport                      m_featureSupport;
		FrameArray<Fence>                          m_fences;
		ComPtr<DXGI::Factory>                      m_factory;
		ComPtr<DX12::Device>                       m_device;
		ComPtr<DX12::CommandQueue>                 m_cmdQueue;
		FrameArray<ComPtr<DX12::CommandAllocator>> m_cmdAllocators;
		ComPtr<DX12::GraphicsCommandList>          m_cmdList;
		DescriptorHeap                             m_rtvHeap;
	};
}
