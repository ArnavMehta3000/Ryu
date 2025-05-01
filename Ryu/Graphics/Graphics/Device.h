#pragma once
#include "Graphics/CommandContext.h"
#include "Graphics/GraphicsConfig.h"

namespace Ryu::Gfx
{
	class Device : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(GFXDevice);

#if defined(RYU_BUILD_DEBUG)
		struct DebugLayer 
		{
			DebugLayer(); 
			~DebugLayer();

			void SetupSeverityBreaks(ComPtr<DX12::Device>& device, bool enable);
			void ReportLiveDeviceObjectsAndReleaseDevice(ComPtr<DX12::Device>& device);
		};
#endif

	public:
		Device();
		~Device();

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD CommandContext* const GetCommandContext() const noexcept { return m_cmdCtx.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }

	private:
		void CreateDevice();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
#if defined(RYU_BUILD_DEBUG)
		DebugLayer                  m_debugLayer;
#endif
		CD3DX12FeatureSupport       m_featureSupport;
		ComPtr<DXGI::Factory>       m_factory;
		ComPtr<DX12::Device>        m_device;
		Memory::Ref<CommandContext> m_cmdCtx;
	};
}
