#pragma once
#include "Common/Result.h"
#include "Common/ObjectMacros.h"
#include "GraphicsRHI/ISwapChain.h"
#include "GraphicsRHI/IDevice.h"
#include "GraphicsRHI/ICommandList.h"
#include "GraphicsRHI/IRenderer.h"
#include "GraphicsRHI/RenderPasses/IClearRenderPass.h"
#include "GraphicsRHI/RenderPasses/IRenderPassFactory.h"

namespace Ryu::Graphics
{
	class Renderer : public IRenderer
	{
		RYU_LOG_CATEGORY(Renderer);
	public:
		NODISCARD VoidResult Init(const SwapChainDesc& swapChainDesc);
		void Shutdown();

		inline ISwapChain* GetSwapChain() const override { return m_swapchain.get(); }
		inline IDevice* GetDevice() const override { return m_device.get(); }

		void InitializeResource(IGraphicsObject* obj) override;
		void BeginFrame();
		void EndFrame();

	private:
		IDevice::CreateDeviceResult CreateDevice();
		void CreateRenderPasses();

	private:
		std::unique_ptr<IDevice>            m_device;
		std::unique_ptr<ISwapChain>         m_swapchain;
		std::unique_ptr<ICommandList>       m_commandList;		
		std::unique_ptr<IRenderPassFactory> m_renderPassFactory;
		std::unique_ptr<IClearRenderPass>   m_backBufferClearPass;
	};
	
	NODISCARD VoidResult InitGraphics(Renderer* renderer, HWND hWnd);
	void ShutdownGraphics(Renderer* renderer);
}
