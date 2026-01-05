#pragma once
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxCommandQueue.h"
#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxTexture.h"

namespace Ryu::Gfx
{
	class Device
	{
	public:
		Device(HWND window);
		~Device();

		void Initialize();

		[[nodiscard]] inline HWND GetWindow() const noexcept { return m_window; }
		
		[[nodiscard]] inline u32 GetFrameIndex() const noexcept { return m_frameIndex; }
		[[nodiscard]] inline const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return m_featureSupport; }
		[[nodiscard]] inline DX12::Device* GetNativeDevice() const noexcept { return m_device.Get(); }
		[[nodiscard]] inline DXGI::Factory* GetFactory() const noexcept { return m_factory.Get(); }
		[[nodiscard]] inline CommandList* GetGraphicsCommandList() const noexcept { return m_cmdList.get(); }
		[[nodiscard]] inline CommandQueue* GetCommandQueue() const noexcept { return m_cmdQueue.get(); }
		[[nodiscard]] inline Texture* GetCurrentBackBuffer() const noexcept { return m_renderTargets[m_frameIndex].get(); }

		[[nodiscard]] std::pair<u32, u32> GetClientSize() const;

		void AddDeviceChild(DeviceChild* deviceChild);
		void RemoveDeviceChild(DeviceChild* deviceChild);

		void BeginFrame(PipelineState* pipelineState = nullptr);
		void EndFrame();
		void Present();

		void WaitForGPU();
		void MoveToNextFrame();

		void ResizeBuffers(u32 w, u32 h);

		void SetBackBufferRenderTarget(bool shouldClear);

	private:
		void CreateDevice();
		void CreateSwapChain();
		void CreateFrameResources(bool isResizing);

	private:
		CD3DX12FeatureSupport                m_featureSupport;
		bool                                 m_isWarpDevice = false;
		bool                                 m_supportsTearing = false;

		HWND                                 m_window;
		u32                                  m_width = 0;
		u32                                  m_height = 0;

		ComPtr<DX12::Device>                 m_device;
		ComPtr<DXGI::Factory>                m_factory;
		ComPtr<DXGI::SwapChain>              m_swapChain;

		std::unique_ptr<DescriptorHeap>      m_rtvHeap;
		std::unique_ptr<CommandQueue>        m_cmdQueue;
		std::unique_ptr<CommandList>         m_cmdList;
		std::unique_ptr<Fence>               m_fence;

		u32                                  m_frameIndex = 0;
		FrameArray<u64>                      m_fenceValues{};

		FrameArray<std::unique_ptr<Texture>> m_renderTargets;

		std::vector<DeviceChild*>            m_deviceChildren;
		CD3DX12_VIEWPORT                     m_viewport;
		CD3DX12_RECT                         m_scissorRect;
	};
}
