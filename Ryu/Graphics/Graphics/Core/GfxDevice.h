#pragma once
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxCommandQueue.h"
#include "Graphics/Core/GfxDescriptorHeap.h"
#include "Graphics/Core/GfxFence.h"
#include <vector>

namespace Ryu::Gfx
{
	class GfxDevice
	{
	public:
		GfxDevice(HWND window);
		~GfxDevice();

		void Initialize();

		[[nodiscard]] inline HWND GetWindow() const noexcept { return m_window; }
		[[nodiscard]] inline bool IsDebugLayerEnabled() const noexcept { return m_isDebugLayerEnabled; }
		[[nodiscard]] inline bool IsValidationEnabled() const noexcept { return m_isValidationEnabled; }
		
		[[nodiscard]] inline u32 GetFrameIndex() const noexcept { return m_frameIndex; }
		[[nodiscard]] inline DX12::Device* GetNativeDevice() const noexcept { return m_device.Get(); }
		[[nodiscard]] inline DXGI::Factory* GetFactory() const noexcept { return m_factory.Get(); }
		[[nodiscard]] inline GfxCommandList* GetGraphicsCommandList() const noexcept { return m_cmdList.get(); }
		[[nodiscard]] inline GfxCommandQueue* GetCommandQueue() const noexcept { return m_cmdQueue.get(); }
		[[nodiscard]] inline DX12::Resource* GetCurrentBackBuffer() const noexcept { return m_renderTargets[m_frameIndex].Get(); }

		[[nodiscard]] std::pair<u32, u32> GetClientSize() const;

		void AddDeviceChild(GfxDeviceChild* deviceChild);
		void RemoveDeviceChild(GfxDeviceChild* deviceChild);

		void BeginFrame();
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
		bool                               m_isDebugLayerEnabled;
		bool                               m_isValidationEnabled;
		bool                               m_isWarpDevice;

		HWND                               m_window;
		u32                                m_width = 0;
		u32                                m_height = 0;

		ComPtr<DX12::Device>               m_device;
		ComPtr<DXGI::Factory>              m_factory;
		ComPtr<DXGI::SwapChain>            m_swapChain;

		std::unique_ptr<GfxDescriptorHeap> m_rtvHeap;
		std::unique_ptr<GfxCommandQueue>   m_cmdQueue;
		std::unique_ptr<GfxCommandList>    m_cmdList;
		std::unique_ptr<GfxFence>          m_fence;

		u32                                m_frameIndex = 0;
		FrameArray<u64>                    m_fenceValues{};

		ComFrameArray<DX12::Resource>      m_renderTargets;

		std::vector<GfxDeviceChild*>       m_deviceChildren;
		CD3DX12_VIEWPORT                   m_viewport;
		CD3DX12_RECT                       m_scissorRect;
	};
}
