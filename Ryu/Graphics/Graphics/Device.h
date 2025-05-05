#pragma once
#include "Graphics/CommandContext.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GraphicsConfig.h"
#include <vector>

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

		void SetDeferredReleaseFlag();
		bool CheckDeferredReleaseFlag(const u32 frameIndex) const;
		void ProcessDeferredReleases(const u32 frameIndex);
		void DeferReleaseObject(DX12::Object* object);  // Called by detaching the ComPtr

	private:
		void CreateDevice();
		void CreateDescriptorHeaps();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
#if defined(RYU_BUILD_DEBUG)
		DebugLayer                  m_debugLayer;
#endif
		CD3DX12FeatureSupport       m_featureSupport;
		ComPtr<DXGI::Factory>       m_factory;
		ComPtr<DX12::Device>        m_device;
		Memory::Ref<CommandContext> m_cmdCtx;
		u32                         m_deferredReleaseFlag[FRAME_BUFFER_COUNT]{};
		std::mutex                  m_deferredReleaseMutex;
		Memory::Ref<DescriptorHeap> m_rtvHeap;
		Memory::Ref<DescriptorHeap> m_dsvHeap;
		Memory::Ref<DescriptorHeap> m_srvHeap;
		Memory::Ref<DescriptorHeap> m_uavHeap;
		std::vector<DX12::Object*>  m_deferredReleases[FRAME_BUFFER_COUNT];
	};
}
