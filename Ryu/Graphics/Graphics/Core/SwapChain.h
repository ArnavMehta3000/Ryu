#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle;

	class SwapChain : public DeviceObject<SwapChain>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		SwapChain() = default;
		SwapChain(std::weak_ptr<Device> parent, HWND window, Format format = BACK_BUFFER_FORMAT);
		~SwapChain();

		inline NODISCARD HWND GetWindowHandle() const noexcept { return m_window; }
		inline NODISCARD DXGI::SwapChain* const GetSwapChain() const noexcept { return m_swapChain.Get(); }
		inline NODISCARD const Format GetFormat() const noexcept { return m_format; }
		inline NODISCARD const CD3DX12_RECT& GetScissorRect() const noexcept { return m_scissorRect; }
		inline NODISCARD const CD3DX12_VIEWPORT& GetViewport() const noexcept { return m_viewport; }

		void Resize(const u32 width, const u32 height);
		void Present() const;

	private:
		void OnConstruct(HWND window, Format format = BACK_BUFFER_FORMAT);
		void CreateSwapChain();
		void CreateFrameResources();

	private:
		HWND                    m_window{ nullptr };
		Format                  m_format{ BACK_BUFFER_FORMAT };
		bool                    m_allowTearing;
		u32                     m_frameCount;
		u32                     m_width;
		u32                     m_height;
		mutable u32             m_frameIndex;
		u32                     m_rtvDescriptorSize;
		ComPtr<DXGI::SwapChain> m_swapChain;
		CD3DX12_VIEWPORT        m_viewport;
		CD3DX12_RECT            m_scissorRect;
	};
}
