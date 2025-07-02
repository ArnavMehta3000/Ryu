#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/CommandList.h"
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
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD CommandCtx* const GetCommandContext() const noexcept { return m_cmdCtx.get(); }
		inline NODISCARD CommandList* const GetCommandList() const noexcept { return m_cmdList.get(); }
		inline NODISCARD Fence* const GetFence() const noexcept { return m_fence.get(); }

		void WaitForGPU();
		void FlushCommandQueue();

	private:
		void Initialize();
		void CreateDevice();
		void CreateCommandContext();
		void CreateCommandList();
		void CreateSynchronization();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		CD3DX12FeatureSupport       m_featureSupport;
		ComPtr<DXGI::Factory>       m_factory;
		ComPtr<DX12::Device>        m_device;
		std::unique_ptr<CommandCtx> m_cmdCtx;
		std::unique_ptr<CommandList> m_cmdList;
		std::unique_ptr<Fence> m_fence;
	};
}
