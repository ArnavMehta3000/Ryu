#include "Renderer.h"
#include "Graphics/Config.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	VoidResult InitGraphics(Renderer* renderer, API api, HWND hWnd)
	{
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return MakeResultError{ "Renderer is nullptr!" };
		}

		const GraphicsConfig& config = GraphicsConfig::Get();

		const DeviceCreateDesc deviceCreateDesc
		{
			.GraphicsAPI              = api,
			.WindowHandle             = hWnd,
			.EnableDebugLayer         = config.EnableDebugLayer,
			.EnableGPUBasedValidation = config.EnableGPUBasedValidation,
			.EnableVSync              = config.EnableVSync
		};
		
		const SwapChainDesc swapChainDesc
		{
			.Width       = 1280,
			.Height      = 720,
			.BufferCount = 2,
			.Format      = Format::R8G8B8A8_UNORM,
			.SwapEffect  = SwapEffect::FlipDiscard
		};

		return renderer->Init(deviceCreateDesc, swapChainDesc);
	}

	void ShutdownGraphics(Renderer* renderer)
	{
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return;
		}
	}

	VoidResult Renderer::Init(const DeviceCreateDesc& deviceCreatedesc, const SwapChainDesc& swapChainDesc)
	{
		auto deviceResult = IDevice::Create(deviceCreatedesc);
		if (!deviceResult)
		{
			return MakeResultError{ deviceResult.error() };
		}

		m_device = std::move(*deviceResult);

		auto swapChainResult = m_device->CreateSwapChain(swapChainDesc);
		if (!swapChainResult)
		{
			return MakeResultError{ swapChainResult.error() };
		}

		m_swapchain = std::move(*swapChainResult);
		
		return {};
	}
	
	void Renderer::Shutdown()
	{
		
	}
	
	void Renderer::InitializeResource(IGraphicsObject* obj)
	{
		if (obj)
		{
			DEBUG_ASSERT(obj->GetRenderer() == nullptr, "Graphics object already has a renderer!");
			obj->SetRenderer(shared_from_this());
		}
	}
}
