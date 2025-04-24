#pragma once
#include "Graphics/DeviceResource.h"
#include "Graphics/Fence.h"

namespace Ryu::Gfx
{
	enum class DisplayMode
	{
		SDR,
		HDR_PQ,
		HDR_scRGB
	};

	class SwapChain : public DeviceObject
	{
	public:
		explicit SwapChain(Device* parent, DisplayMode displayMode, u32 frameCount, HWND window);
		~SwapChain();

		void OnResize(u32 width, u32 height);
		void Present();

		void SetFrameCount(u32 frameCount);
		void SetMaxFrameLatency(u32 maxFrameLatency);
		void SetUseWaitableSwapChain(bool enabled);

		bool DisplaySupportsHDR() const;

		inline bool GetUseWaitableSwapChain() { return m_useWaitableObject; }
		inline NODISCARD u32 GetFrameCount() const { return m_frameCount; }
		inline NODISCARD u32 GetMaxFrameLatency() const { return m_maxFrameLatency; }
		inline NODISCARD Format GetFormat() const { return m_format; }
		inline NODISCARD u32 GetBackBufferIndex() const { return m_backBufferIndex; }
		inline NODISCARD DXGI::SwapChain* GetSwapChain() const { return m_swapChain.Get(); }

	private:
		void RecreateSwapChain();

	private:
		HWND                    m_window;
		DisplayMode             m_displayMode;
		Format                  m_format;
		std::shared_ptr<Fence>  m_presentFence;
		RYU_TODO("Add std::array<ComPtr<Texture>> m_backBuffers");
		ComPtr<DXGI::SwapChain> m_swapChain;
		u32                     m_backBufferIndex;
		u32                     m_width = 0;
		u32                     m_height = 0;
		u32                     m_frameCount;
		u32                     m_maxFrameLatency = 2;
		bool                    m_useWaitableObject = true;
		bool                    m_AllowTearing = false;
		HANDLE                  m_waitableObject = nullptr;
	};
}
