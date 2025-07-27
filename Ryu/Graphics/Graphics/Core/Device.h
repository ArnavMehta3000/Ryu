#pragma once
#include "Graphics/Core/DeviceObject.h"
#include "Graphics/Core/CommandContext.h"

namespace Ryu::Gfx
{
	class Renderer;

	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		static DevicePtr Create();
		static void Destroy(Device& device);

		inline NODISCARD DX12::Device* const GetDevice() const noexcept { return m_device.Get(); }
		inline NODISCARD DXGI::Factory* const GetFactory() const noexcept { return m_factory.Get(); }
		inline NODISCARD const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		inline NODISCARD Command& GetCommand() { return m_command; }

		inline u32 GetCurrentFrameIndex() const { return m_command.GetFrameIndex(); }
		inline void SetDeferredReleaseFlag() { m_deferredReleaseFlags[m_command.GetFrameIndex()] = 1; }
		void ProcessDeferredReleases(u32 frameIndex);
		void DeferredRelease(IUnknown* resource);

		// Process releases if needed at the start
		void BeginFrame(PipelineState* pso);
		void EndFrame();

	private:
		void Initialize();
		void CreateDevice();
		void GetHardwareAdapter(DXGI::Factory* pFactory, DXGI::Adapter** ppAdapter) const;

	private:
		CD3DX12FeatureSupport              m_featureSupport;		
		ComPtr<DXGI::Factory>              m_factory;
		ComPtr<DX12::Device>               m_device;
		Command                            m_command;
		FrameArray<u32>                    m_deferredReleaseFlags;
		FrameArray<std::vector<IUnknown*>> m_deferredReleases;
	};
}
