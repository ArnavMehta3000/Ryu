#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/Fence.h"

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
		inline NODISCARD DX12::CommandList* const GetCommandList() const noexcept { return m_cmdList.Get(); }
		inline NODISCARD DX12::CommandAllocator* const GetCommandAllocator() const noexcept { return m_cmdAllocator.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD const Fence& GetFence() const noexcept { return m_fence; }

		void WaitForGPU();
		void FlushCommandQueue();

	private:
		void Initialize();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateCommandList();
		void CreateSynchronization();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		CD3DX12FeatureSupport          m_featureSupport;
		Fence                          m_fence;
		ComPtr<DXGI::Factory>          m_factory;
		ComPtr<DX12::Device>           m_device;
		ComPtr<DX12::CommandQueue>     m_cmdQueue;
		ComPtr<DX12::CommandAllocator> m_cmdAllocator;
		ComPtr<DX12::CommandList>      m_cmdList;
	};
}
