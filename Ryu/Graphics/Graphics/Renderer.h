#pragma once
#include "Common/Result.h"
#include "Common/ObjectMacros.h"
#include "Graphics/Shared/Interface/ISwapChain.h"
#include "Graphics/Shared/Interface/IDevice.h"

namespace Ryu::Graphics
{
	class Renderer
	{
	public:
		NODISCARD VoidResult Init(const DeviceCreateDesc& deviceCreatedesc, const SwapChainDesc& swapChainDesc);
		void Shutdown();

		void InitializeResource(IGraphicsObject* obj);

	private:
		std::unique_ptr<ISwapChain> m_swapchain;
		std::unique_ptr<IDevice> m_device;
	};
	
	NODISCARD VoidResult InitGraphics(Renderer* renderer, API api, HWND hWnd);
	void ShutdownGraphics(Renderer* renderer);
}
