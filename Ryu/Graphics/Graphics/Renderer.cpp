#include "Renderer.h"
#include "Graphics/Config.h"
#include "Graphics/Shared/Logging.h"
#include <libassert/assert.hpp>
#include <dxgi1_6.h>
#include <dxgidebug.h>

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
			.EnableDebugLayer         = config.EnableDebugLayer,
			.EnableGPUBasedValidation = config.EnableGPUBasedValidation,
			.EnableVSync              = config.EnableVSync
		};
		
		const SwapChainDesc swapChainDesc
		{
			.Width         = 1280,
			.Height        = 720,
			.BufferCount   = 2,
			.SampleCount   = 1,
			.SampleQuality = 0,
			.Flags         = 0,
			.Format        = Format::R8G8B8A8_UNORM,
			.SwapEffect    = SwapEffect::FlipDiscard,
			.WindowHandle  = hWnd,
			.Windowed      = true
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
		DXCall(::CoInitialize(NULL));

		// Enable DXGI debug layer if requested
		if (deviceCreatedesc.EnableDebugLayer)
		{
			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			}
		}

		auto deviceResult = IDevice::Create(deviceCreatedesc);
		if (!deviceResult)
		{
			return MakeResultError{ deviceResult.error() };
		}

		m_device = std::move(*deviceResult);
		InitializeResource(m_device.get());

		auto swapChainResult = m_device->CreateSwapChain(swapChainDesc);
		if (!swapChainResult)
		{
			return MakeResultError{ swapChainResult.error() };
		}

		m_swapchain = std::move(*swapChainResult);
		InitializeResource(m_swapchain.get());
		
		return {};
	}
	
	void Renderer::Shutdown()
	{
		::CoUninitialize();
	}
	
	void Renderer::InitializeResource(IGraphicsObject* obj)
	{
		if (obj)
		{
			DEBUG_ASSERT(obj->GetRenderer() == nullptr, "Graphics object already has a renderer!");
			
			if (obj->GetRenderer() != nullptr)
			{
				return;
			}

			obj->SetRenderer(shared_from_this());
		}
	}
}
