#pragma once
#include "Common/Result.h"
#include "Common/ObjectMacros.h"
#include "Graphics/Shared/Interface/ISwapChain.h"
#include "Graphics/Shared/Interface/IDevice.h"
#include "Graphics/Shared/Interface/ICommandList.h"
#include <vector>

namespace Ryu::Graphics
{
	class Renderer
	{
	public:
		NODISCARD VoidResult Init(const DeviceCreateDesc& deviceCreatedesc, const SwapChainDesc& swapChainDesc);
		void Shutdown();

		NODISCARD inline ISwapChain* GetSwapChain() const { return m_swapchain.get(); }
		NODISCARD inline IDevice* GetDevice() const { return m_device.get(); }

		void InitializeResource(IGraphicsObject* obj);
		void BeginFrame();
		void EndFrame();

	private:
		std::unique_ptr<ISwapChain> m_swapchain;
		std::unique_ptr<IDevice> m_device;
		std::vector<std::unique_ptr<ICommandList>> m_commandLists;
	};
	
	NODISCARD VoidResult InitGraphics(Renderer* renderer, API api, HWND hWnd);
	void ShutdownGraphics(Renderer* renderer);
}
