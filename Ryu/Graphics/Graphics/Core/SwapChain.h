#pragma once
#include "Graphics/Core/DescriptorHeap.h"

namespace Ryu::Gfx
{
	struct RenderSurface
	{
		ComPtr<DX12::Resource> Resource;
		DescriptorHandle RTV{};
	};

	class SwapChain final : public DeviceObject<SwapChain>
	{
		RYU_DISABLE_COPY_AND_MOVE(SwapChain)
		RYU_GFX_DEVICE_OBJ;
	public:
		SwapChain() = default;
		SwapChain(std::weak_ptr<Device> parent, CommandQueue& queue, DescHeap& rtvHeap, HWND window, Format format = BACK_BUFFER_FORMAT);
		~SwapChain();

		inline NODISCARD HWND GetWindowHandle() const noexcept { return m_window; }
		inline NODISCARD DXGI::SwapChain* const GetSwapChain() const noexcept { return m_swapChain.Get(); }
		inline NODISCARD const Format GetFormat() const noexcept { return m_format; }
		inline NODISCARD const CD3DX12_RECT& GetScissorRect() const noexcept { return m_scissorRect; }
		inline NODISCARD const CD3DX12_VIEWPORT& GetViewport() const noexcept { return m_viewport; }
		inline NODISCARD const RenderSurface& GetRenderSurface(const u32 index) const { return m_surfaceData[index]; }
		inline NODISCARD u32 GetFrameIndex() const noexcept { return m_frameIndex; }

		void Resize(const u32 width, const u32 height);
		void Present() const;

	private:
		void OnConstruct(CommandQueue& queue, DescHeap& rtvHeap, HWND window, Format format = BACK_BUFFER_FORMAT);
		void OnDestruct();
		void CreateSwapChain(CommandQueue& queue);
		void CreateFrameResources();

	private:
		HWND                      m_window{ nullptr };
		Format                    m_format{ BACK_BUFFER_FORMAT };
		u32                       m_width;
		u32                       m_height;
		mutable u32               m_frameIndex;
		u32                       m_rtvDescriptorSize;
		DescHeap*                 m_rtvHeap;
		ComPtr<DXGI::SwapChain>   m_swapChain;
		FrameArray<RenderSurface> m_surfaceData;
		CD3DX12_VIEWPORT          m_viewport;
		CD3DX12_RECT              m_scissorRect;
	};
}
