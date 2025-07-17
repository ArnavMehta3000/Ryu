#pragma once
#include "Logger/LogCategory.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/SwapChain.h"

namespace Ryu::Gfx
{
	class Renderer
	{
	public:
		Renderer() = delete;
		explicit Renderer(HWND window);
		~Renderer();

		inline NODISCARD const Device& GetDevice() const { return *m_device; }
		inline NODISCARD const SwapChain& GetSwapChain() const { return m_swapChain; }

		void Render(/*Scene, Camera*/);
		void OnResize(u32 width, u32 height);

	private:
		void PopulateCommandList();

	private:
		DevicePtr m_device;
		SwapChain m_swapChain;
	};
}
