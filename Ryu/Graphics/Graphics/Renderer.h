#pragma once
#include "Common/Result.h"
#include "Logger/LogCategory.h"
#include "Graphics/Device.h"
#include "Graphics/SwapChain.h"

namespace Ryu::Gfx
{
	class Renderer
	{
		RYU_LOG_DECLARE_CATEGORY(Renderer);

	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer();

		inline NODISCARD Device* GetDevice() const { return m_device.get(); }
		inline NODISCARD SwapChain* GetSwapChain() const { return m_swapchain.get(); }

		void OnResize(u32 width, u32 height);

	public:
		std::unique_ptr<Device> m_device;
		std::unique_ptr<SwapChain> m_swapchain;
	};
}
