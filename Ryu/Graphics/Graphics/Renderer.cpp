#include "Renderer.h"
#include "Graphics/RHIEnabled.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/GraphicsConfig.h"
#include "GraphicsRHI/Utils/Logging.h"
#if RYU_RHI_HAS_D3D11
#include "GraphicsDX11/DX11Device.h"
#include "GraphicsDX11/RenderPasses/DX11RenderPassFactory.h"
#endif
#if RYU_RHI_HAS_D3D12
#include "GraphicsDX12/DX12Device.h"
#endif
#include <libassert/assert.hpp>


namespace Ryu::Graphics
{
	VoidResult InitGraphics(Renderer* renderer, HWND hWnd)
	{
		RYU_PROFILE_SCOPE();
		RYU_LOG_CATEGORY(Graphics);

		if (!renderer)
		{
			return MakeResultError{ "Renderer is nullptr!" };
		}

		const GraphicsConfig& config = GraphicsConfig::Get();

		// Check for API support
#if !defined(RYU_RHI_D3D12)  // D3D12 not enabled, but requested
		if (config.GraphicsAPI.Get() == API::DirectX12)
		{
			return MakeResultError{ std::format("Trying to initialize Renderer with {} failed as it is not enabled via build config.", EnumToString(config.GraphicsAPI.Get())) };
		}
#endif
#if !defined(RYU_RHI_D3D11)  // D3D11 not enabled, but requested
		if (config.GraphicsAPI.Get() == API::DirectX11)
		{
			return MakeResultError{ std::format("Trying to initialize Renderer with {} failed as it is not enabled via build config.", EnumToString(config.GraphicsAPI.Get())) };
		}
#endif

		// If GPU based validation is enabled automatically enable the debug layer as well
		if (config.EnableGPUBasedValidation && !config.EnableDebugLayer)
		{
			LOG_WARN(RYU_USE_LOG_CATEGORY(Graphics), "GPU based validation is enabled, automatically enabling graphics debug layer. GraphicsConfig will be updated");
			const_cast<GraphicsConfig&>(config).EnableDebugLayer.Set(true);
		}


		const SwapChainDesc swapChainDesc
		{
			.Width         = 1280,
			.Height        = 720,
			.BufferCount   = GraphicsConfig::FRAME_COUNT,
			.SampleCount   = 1,
			.SampleQuality = 0,
			.Flags         = SwapChainFlag::AllowModeSwitch | SwapChainFlag::AllowTearing,
			.Format        = Format::R8G8B8A8_UNORM,
			.SwapEffect    = SwapEffect::FlipSequential,
			.WindowHandle  = hWnd,
			.Windowed      = true
		};

		return renderer->Init(swapChainDesc);
	}

	void ShutdownGraphics(Renderer* renderer)
	{
		RYU_PROFILE_SCOPE();
		DEBUG_ASSERT(renderer, "Renderer is nullptr!");
		if (!renderer)
		{
			return;
		}

		renderer->Shutdown();
	}


	VoidResult Renderer::Init(const SwapChainDesc& swapChainDesc)
	{
		RYU_PROFILE_SCOPE();
		DXCall(::CoInitializeEx(NULL, COINIT_MULTITHREADED));


		auto deviceResult = CreateDevice();
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
		
		CreateRenderPasses();
		
		return {};
	}
	
	void Renderer::Shutdown()
	{
		RYU_PROFILE_SCOPE();
		m_renderPassFactory.reset();
		m_backBufferClearPass.reset();

		m_commandList.reset();
		m_swapchain.reset();
		m_device.reset();
	
		::CoUninitialize();
	}
	
	void Renderer::InitializeResource(IGraphicsObject* obj)
	{
		RYU_PROFILE_SCOPE();
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
		RYU_PROFILE_SCOPE();
		m_commandList->Begin();

		m_backBufferClearPass->Begin(m_commandList.get());
		m_backBufferClearPass->End(m_commandList.get());

		m_commandList->End();

		m_device->ExecuteCommandList(m_commandList.get());
	}
	
	void Renderer::EndFrame()
	{
		RYU_PROFILE_SCOPE();
		m_swapchain->Present();
	}
	
	IDevice::CreateDeviceResult Renderer::CreateDevice()
	{
		RYU_PROFILE_SCOPE();
		
		const API api = GraphicsConfig::Get().GraphicsAPI.Get();

		switch (api)
		{
#if RYU_RHI_HAS_D3D11
		case API::DirectX11:
			return std::make_unique<DX11::DX11Device>();
#endif
#if RYU_RHI_HAS_D3D12
		case API::DirectX12:
			return std::make_unique<DX12::DX12Device>();
#endif
		case API::Undefined:
		default:
			return MakeResultError{ std::format(
				"Unsupported graphics device API - {}", EnumToString(api)) };
		}
	}
	
	void Renderer::CreateRenderPasses()
	{
		RYU_PROFILE_SCOPE();

		const API api = GraphicsConfig::Get().GraphicsAPI.Get();
		
		if (!m_renderPassFactory)
		{
			switch (api)
			{
#if RYU_RHI_HAS_D3D11
			case API::DirectX11:
				m_renderPassFactory = std::make_unique<DX11::DX11RenderPassFactory>();
				break;
#endif
#if RYU_RHI_HAS_D3D12
			case API::DirectX12:
				m_renderPassFactory = std::make_unique<DX11::DX12RenderPassFactory>();
				break;
#endif
			case API::Undefined:
			default:
				LOG_ERROR(RYU_USE_LOG_CATEGORY(Renderer), "Render pass factory for API {} is not defined", EnumToString(api));
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
