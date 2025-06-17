#pragma once
#include "Graphics/Core/DeviceObject.h"

namespace Ryu::Gfx
{
	struct DescriptorHandle;

	class SwapChain : public DeviceObject
	{
		RYU_LOG_DECLARE_CATEGORY(GFXSwapChain);

		struct SurfaceData
		{
			ComPtr<DX12::Resource> Resource = nullptr;
			DescriptorHandle RTV{};
		};

	public:
		static constexpr Format DEFAULT_RTV_FORMAT = Format::RGBA8_UNORM;

		static std::shared_ptr<SwapChain> Create(std::weak_ptr<Device> parent, HWND window, Format format = DEFAULT_RTV_FORMAT);

		~SwapChain();

		inline NODISCARD HWND GetWindowHandle() const noexcept { return m_window; }
		inline NODISCARD DXGI::SwapChain* const GetSwapChain() const noexcept { return m_swapChain.Get(); }
		inline NODISCARD const Format GetFormat() const noexcept { return m_format; }
		inline NODISCARD const SurfaceData& GetCurrentSurfaceData() const noexcept { return m_surfaceData[m_frameIndex]; }
		inline NODISCARD const CD3DX12_RECT& GetScissorRect() const noexcept { return m_scissorRect; }
		inline NODISCARD const CD3DX12_VIEWPORT& GetViewport() const noexcept { return m_viewport; }

		void Resize(const u32 width, const u32 height);
		void Present() const;

	private:
		SwapChain(std::weak_ptr<Device> parent, HWND window, Format format);
		void CreateSwapChain();
		void CreateFrameResources();

	private:
		HWND                    m_window;
		Format                  m_format;
		bool                    m_allowTearing;
		u32                     m_frameCount;
		u32                     m_width;
		u32                     m_height;
		mutable u32             m_frameIndex;
		u32                     m_rtvDescriptorSize;
		ComPtr<DXGI::SwapChain> m_swapChain;
		SurfaceData             m_surfaceData[FRAME_BUFFER_COUNT];
		CD3DX12_VIEWPORT        m_viewport;
		CD3DX12_RECT            m_scissorRect;
	};
}
