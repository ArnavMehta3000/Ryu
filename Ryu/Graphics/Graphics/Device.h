#pragma once
#include "Graphics/DeviceResource.h"
#include "Graphics/GraphicsConfig.h"

namespace Ryu::Gfx
{
	class Device : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(GFXDevice);

		struct DebugLayer 
		{
			DebugLayer(); 
			~DebugLayer();

			void SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable);
		};

	public:
		Device();
		~Device();

		inline NODISCARD DX12::Device* GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD DX12::CmdQueue* GetCmdQueue() const noexcept { return m_cmdQueue.Get(); }
		inline NODISCARD DX12::CmdAllocator* GetCmdAllocator() const noexcept { return m_cmdAllocator.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }

	private:
		void CreateDevice();
		void CreateCommandQueue();
		void CreateCommandAllocator();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		DebugLayer                 m_debugLayer;
		CD3DX12FeatureSupport      m_featureSupport;
		ComPtr<DXGI::Factory>      m_factory;
		ComPtr<DX12::Device>       m_device;
		ComPtr<DX12::CmdQueue>     m_cmdQueue;
		ComPtr<DX12::CmdAllocator> m_cmdAllocator;
	};
}
