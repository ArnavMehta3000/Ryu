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

		inline NODISCARD Device* GetDevice() const { return m_device.Get(); }
		inline NODISCARD SwapChain* GetSwapChain() const { return m_swapchain.Get(); }

		void OnResize(u32 width, u32 height);

	public:
		Memory::Ref<Device>    m_device;
		Memory::Ref<SwapChain> m_swapchain;
	};
}
