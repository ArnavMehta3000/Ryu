#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/GraphicsConfig.h"
#include <vector>

namespace Ryu::Gfx
{
	class Device : public std::enable_shared_from_this<Device>
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
		static std::shared_ptr<Device> Create();

		~Device();

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD auto const GetCommandContext() const noexcept { return m_cmdCtx; }
		inline NODISCARD auto const GetRTVHeap() const noexcept { return m_rtvHeap; }
		inline NODISCARD auto const GetDSVHeap() const noexcept { return m_dsvHeap; }
		inline NODISCARD auto const GetSRVHeap() const noexcept { return m_srvHeap; }
		inline NODISCARD auto const GetUAVHeap() const noexcept { return m_uavHeap; }

		void SetDeferredReleaseFlag();
		bool CheckDeferredReleaseFlag(const u32 frameIndex) const;
		void ProcessDeferredReleases(const u32 frameIndex);
		void DeferReleaseObject(DX12::Object* object);  // Called by detaching the ComPtr

	private:
		Device() = default;
		void Initialize();
		void CreateDevice();
		void CreateDescriptorHeaps();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
#if defined(RYU_BUILD_DEBUG)
		DebugLayer                      m_debugLayer;
#endif
		CD3DX12FeatureSupport           m_featureSupport;
		ComPtr<DXGI::Factory>           m_factory;
		ComPtr<DX12::Device>            m_device;
		std::shared_ptr<CommandContext> m_cmdCtx;
		u32                             m_deferredReleaseFlag[FRAME_BUFFER_COUNT]{};
		std::mutex                      m_deferredReleaseMutex;
		std::shared_ptr<DescriptorHeap> m_rtvHeap;
		std::shared_ptr<DescriptorHeap> m_dsvHeap;
		std::shared_ptr<DescriptorHeap> m_srvHeap;
		std::shared_ptr<DescriptorHeap> m_uavHeap;
		std::vector<DX12::Object*>      m_deferredReleases[FRAME_BUFFER_COUNT];
	};
}
