#pragma once
#include "Logger/LogCategory.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/SwapChain.h"

namespace Ryu::Gfx
{
	class Renderer
	{
		RYU_LOG_DECLARE_CATEGORY(Renderer);

	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer();

		inline NODISCARD const Device& GetDevice() const { return *m_device; }
		inline NODISCARD const SwapChain& GetSwapChain() const { return m_swapChain; }

		void WaitForNextFrameResources();
		void WaitForLastSubmittedFrame();

		void Render(/*Scene, Camera*/);
		void OnResize(u32 width, u32 height);

	public:
		std::shared_ptr<Device> m_device;
		SwapChain               m_swapChain;
		FrameArray<u64>         m_frameValues;
		u64                     m_lastSubmittedValue = 0;
	};
}
