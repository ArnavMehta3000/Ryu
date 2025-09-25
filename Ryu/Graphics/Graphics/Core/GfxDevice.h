#pragma once
#include "Graphics/Core/GfxFence.h"
#include "Graphics/Core/GfxCommandQueue.h"

namespace Ryu::Gfx
{
	class GfxDevice;

	class GfxSwapChain
	{
	public:
		struct Desc
		{
			u32 Width = 0;
			u32 Height = 0;
			DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		};
	public:
		GfxSwapChain(GfxDevice* device, const GfxSwapChain::Desc& desc);
		~GfxSwapChain();

		inline u32 GetBackBufferIndex() const noexcept { return m_backBufferIndex; }

	private:
		void CreateBackBuffers();

	private:
		GfxDevice* m_device = nullptr;
		ComPtr<DXGI::SwapChain> m_swapChain;
		u32 m_width, n_height;
		u32 m_backBufferIndex;
	};

	class GfxDevice
	{
	public:
		explicit GfxDevice(HWND window);
		~GfxDevice();

		[[nodiscard]] inline HWND GetHWND() const noexcept { return m_hWnd; }
		[[nodiscard]] inline auto GetDevice() const noexcept { return m_device.Get(); }
		[[nodiscard]] inline auto GetFactory() const noexcept { return m_dxgiFactory.Get(); }
		[[nodiscard]] inline auto& GetCapabilities() const noexcept { return m_capabilities; }
		[[nodiscard]] inline GfxFence& GetGraphicsFence() noexcept { return m_graphicsFence; }
		[[nodiscard]] inline GfxFence& GetComputeFence() noexcept { return m_computeFence; }
		[[nodiscard]] inline GfxFence& GetCopyFence() noexcept { return m_copyFence; }
		[[nodiscard]] inline u64 GetGraphicsFenceValue() const noexcept { return m_graphicsFenceValue; }
		[[nodiscard]] inline u64 GetComputeFenceValue() const noexcept { return m_computeFenceValue; }
		[[nodiscard]] inline u64 GetCopyFenceValue() const noexcept { return m_copyFenceValue; }

		[[nodiscard]] GfxCommandQueue& GetCommandQueue(CommandListType type);
		[[nodiscard]] GfxCommandQueue& GetGraphicsCommandQueue() { return GetCommandQueue(CommandListType::Direct); }
		[[nodiscard]] GfxCommandQueue& GetComputeCommandQueue() { return GetCommandQueue(CommandListType::Compute); }
		[[nodiscard]] GfxCommandQueue& GetCopyCommandQueue() { return GetCommandQueue(CommandListType::Copy); }



		inline void SetGraphicsFenceValue(u64 value) { m_graphicsFenceValue = value; }
		inline void SetComputeFenceValue(u64 value) { m_computeFenceValue = value; }
		inline void SetCopyFenceValue(u64 value) { m_copyFenceValue = value; }


		void WaitForGPU();

	private:
		HWND m_hWnd;
		u32  m_width, m_height;
		u32  m_frameIndex = 0;

		ComPtr<DXGI::Factory> m_dxgiFactory;
		ComPtr<DX12::Device>  m_device;
		CD3DX12FeatureSupport m_capabilities;

		std::unique_ptr<GfxSwapChain> m_swapChain;

		GfxCommandQueue m_graphicsQueue;
		GfxCommandQueue m_computeQueue;
		GfxCommandQueue m_copyQueue;

		GfxFence        m_frameFence;
		u64             m_frameFenceValue = 1;
		FrameArray<u64> m_frameFenceValues;

		GfxFence m_graphicsFence;
		u64 m_graphicsFenceValue = 0;

		GfxFence m_computeFence;
		u64 m_computeFenceValue = 0;
		
		GfxFence m_copyFence;
		u64 m_copyFenceValue = 0;

		GfxFence m_waitFence;
		u64 m_waitFenceValue = 1;

		GfxFence m_releaseFence;
		u64 m_releaseFenceValue = 1;
	};
}
