#pragma once
#include "Graphics/DeviceResource.h"

namespace Ryu::Gfx
{
	class SwapChain : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(GFXSwapChain);

	public:
		SwapChain(Device* parent, HWND window);
		~SwapChain() = default;

		inline NODISCARD HWND GetWindowHandle() const noexcept { return m_window; }
		inline NODISCARD DXGI::SwapChain* GetSwapChain() const noexcept { return m_swapChain.Get(); }

		void Resize(const u32 width, const u32 height);

	private:
		void CreateSwapChain();
		void CreateDescriptorHeaps();
		void CreateFrameResources();

	private:
		HWND                    m_window;
		Format                  m_format = Format::RGBA8_UNORM;
		bool                    m_allowTearing;
		u32                     m_frameCount;
		u32                     m_width;
		u32                     m_height;
		u32                     m_frameIndex;
		u32                     m_rtvDescriptorSize;
		ComPtr<DXGI::SwapChain> m_swapChain;
		ComPtr<DX12::DescHeap>  m_rtvHeap;
		ComPtr<DX12::Resource>  m_frameBuffers[FRAME_BUFFER_COUNT];
	};
}
