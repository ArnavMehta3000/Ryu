#include "Renderer.h"
#include "GraphicsRHI/Config.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX11/DX11Device.h"
#include "GraphicsDX11/RenderPasses/DX11RenderPassFactory.h"
#include <libassert/assert.hpp>
#include <dxgi1_6.h>

namespace Ryu::Graphics
{
	VoidResult InitGraphics(Renderer* renderer, API api, HWND hWnd)
	{
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return MakeResultError{ "Renderer is nullptr!" };
		}

		GraphicsConfig& config = GraphicsConfig::Get();

		// If GPU based validation is enabled automatically enable the debug layer as well
		if (config.EnableGPUBasedValidation && !config.EnableDebugLayer)
		{
			config.EnableDebugLayer = true;
		}


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
			.BufferCount   = FRAME_BUFFER_COUNT,
			.SampleCount   = 1,
			.SampleQuality = 0,
			.Flags         = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
			.Format        = Format::R8G8B8A8_UNORM,
			.SwapEffect    = SwapEffect::FlipSequential,
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

		renderer->Shutdown();
	}


	VoidResult Renderer::Init(const DeviceCreateDesc& deviceCreatedesc, const SwapChainDesc& swapChainDesc)
	{
		DXCall(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

		auto deviceResult = CreateDevice(deviceCreatedesc);
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


		CommandListDesc cmdListDesc{ CommandListType::Graphics };

		if (auto createCmdListResult = m_device->CreateCommandList(cmdListDesc))
		{
			m_commandList = std::move(*createCmdListResult);
		}
		else
		{
			return MakeResultError{ "Failed to create command list" };
		}
		
		CreateRenderPasses(deviceCreatedesc);
		
		return {};
	}
	
	void Renderer::Shutdown()
	{
		m_renderPassFactory.reset();
		m_backBufferClearPass.reset();

		m_commandList.reset();
		m_swapchain.reset();
		m_device.reset();
	
		::CoUninitialize();
	}
	
	void Renderer::InitializeResource(IGraphicsObject* obj)
	{
		if (obj)
		{
			DEBUG_ASSERT(obj->GetRendererInterface() == nullptr, "Graphics object already has a renderer!");
			
			if (obj->GetRendererInterface() != nullptr)
			{
				return;
			}

			obj->SetRendererInterface(this);
		}
	}
	
	void Renderer::BeginFrame()
	{
		m_commandList->Begin();

		m_backBufferClearPass->Begin(m_commandList.get());
		m_backBufferClearPass->End(m_commandList.get());

		m_commandList->End();

		m_device->ExecuteCommandList(m_commandList.get());
	}
	
	void Renderer::EndFrame()
	{
		m_swapchain->Present();
	}
	
	IDevice::CreateDeviceResult Renderer::CreateDevice(const DeviceCreateDesc& deviceCreatedesc)
	{
		switch (deviceCreatedesc.GraphicsAPI)
		{
		case API::DirectX11:
			return std::make_unique<DX11::DX11Device>(deviceCreatedesc);
		}

		return MakeResultError{ std::format(
			"Unsupported graphics device API - {}", EnumToString(deviceCreatedesc.GraphicsAPI)) };
	}
	
	void Renderer::CreateRenderPasses(const DeviceCreateDesc& deviceCreatedesc)
	{
		if (!m_renderPassFactory)
		{
			switch (deviceCreatedesc.GraphicsAPI)
			{
			case API::DirectX11:
				m_renderPassFactory = std::make_unique<DX11::DX11RenderPassFactory>();
				break;
			}
		}

		// Create back buffer clear render pass
		if (!m_backBufferClearPass)
		{
			IClearRenderPass::Desc desc;
			desc.RenderTargets = { m_swapchain->GetBackBufferRenderTarget() };
			desc.ClearColor = { 1.0f, 0.0f, 0.0f, 1.0f };

			m_backBufferClearPass = m_renderPassFactory->CreateClearRenderPass(desc);
		}
	}
}
