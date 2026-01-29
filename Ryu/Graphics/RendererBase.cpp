#include "Graphics/RendererBase.h"
#include "Graphics/RendererDX12.h"
#include "Core/Logging/Logger.h"
#include <nvrhi/utils.h>

namespace Ryu::Gfx
{
	RendererBase* RendererBase::Create(nvrhi::GraphicsAPI api)
	{
		switch (api)
		{
		case nvrhi::GraphicsAPI::D3D12: return new RendererDX12();
		case nvrhi::GraphicsAPI::D3D11:
		case nvrhi::GraphicsAPI::VULKAN:
		default:
			nvrhi::utils::NotSupported();
			return nullptr;
		}
	}

	bool RendererBase::Init(const RendererInitInfo& info, HWND targetWindow)
	{
		m_hWnd = targetWindow;
		m_initInfo = info;

		if (!CreateInstance())
		{
			RYU_LOG_ERROR("Failed to create renderer instance!");
			return false;
		}

		if (!CreateDevice())
		{
			RYU_LOG_ERROR("Failed to create renderer device!");
			return false;
		}

		if (!CreateSwapChain())
		{
			RYU_LOG_ERROR("Failed to create swap chain!");
			return false;
		}

		// Force a resize event using invalid width and height
		m_initInfo.BackBufferSize = { 0, 0 };
		
		Resize();

		return false;
	}

	void RendererBase::Shutdown()
	{
		m_swapChainFrameBuffers.clear();
		m_swapChainWithDepthFrameBuffers.clear();
		m_depthBuffer = nullptr;

		DestroyDeviceAndSwapChain();

		m_instanceCreated = false;
	}
	
	nvrhi::IFramebuffer* RendererBase::GetCurrentFrameBuffer(bool withDepth)
	{
		return GetFrameBuffer(GetCurrentBackBufferIndex(), withDepth);
	}

	nvrhi::IFramebuffer* RendererBase::GetFrameBuffer(u32 index, bool withDepth)
	{
		if (withDepth && index < m_swapChainWithDepthFrameBuffers.size())
		{
			return m_swapChainWithDepthFrameBuffers[index];
		}
		else if (index < m_swapChainFrameBuffers.size())
		{

			return m_swapChainFrameBuffers[index];
		}

		return nullptr;
	}

	bool RendererBase::CreateInstance()
	{
		if (m_instanceCreated)
		{
			return true;
		}
		m_instanceCreated = CreateDeviceIndependentResources();
		return m_instanceCreated;
	}

	void RendererBase::BackBufferResizeBegin()
	{
		// Clear swapchain buffers
		m_swapChainFrameBuffers.clear();
		m_swapChainWithDepthFrameBuffers.clear();

		for (IRenderPass* pass : m_renderPasses)
		{
			pass->OnBackBufferResizeBegin();
		}
	}

	void RendererBase::BackBufferResizeEnd()
	{
		CreateDepthBuffer();

		auto& [width, height] = m_initInfo.BackBufferSize;
		for (IRenderPass* pass : m_renderPasses)
		{
			pass->OnBackBufferResizeEnd(width, height, m_initInfo.SwapChainSampleCount);
		}

		const u32 backBufferCount = GetBackBufferCount();
		m_swapChainFrameBuffers.resize(backBufferCount);
		m_swapChainWithDepthFrameBuffers.resize(backBufferCount);

		for (u32 i = 0; i < backBufferCount; i++)
		{
			nvrhi::FramebufferDesc desc = nvrhi::FramebufferDesc()
				.addColorAttachment(GetBackBuffer(i));

			m_swapChainFrameBuffers[i] = GetDevice()->createFramebuffer(desc);

			if (m_depthBuffer)
			{
				desc.setDepthAttachment(m_depthBuffer);
				m_swapChainWithDepthFrameBuffers[i] = GetDevice()->createFramebuffer(desc);
			}
			else
			{
				m_swapChainWithDepthFrameBuffers[i] = m_swapChainFrameBuffers[i];
			}
		}
	}

	void RendererBase::CreateDepthBuffer()
	{
		m_depthBuffer = nullptr;

		if (m_initInfo.DepthBufferFormat == nvrhi::Format::UNKNOWN)
		{
			RYU_LOG_DEBUG("Skipping depth buffer creation, format is unknown");
			return;
		}

		auto& [width, height] = m_initInfo.BackBufferSize;
		const auto textureDesc = nvrhi::TextureDesc()
			.setDebugName("Depth Buffer")
			.setWidth(width)
			.setHeight(height)
			.setFormat(m_initInfo.DepthBufferFormat)
			.setDimension(m_initInfo.SwapChainSampleCount > 1
				? nvrhi::TextureDimension::Texture2DMS
				: nvrhi::TextureDimension::Texture2D)
			.setSampleCount(m_initInfo.SwapChainSampleCount)
			.setSampleQuality(m_initInfo.SwapChainSampleQuality)
			.setIsTypeless(true)
			.setIsRenderTarget(true)
			.enableAutomaticStateTracking(nvrhi::ResourceStates::DepthWrite);

		m_depthBuffer = GetDevice()->createTexture(textureDesc);
	}
	
	void RendererBase::message(nvrhi::MessageSeverity severity, const char* messageText)
	{
		switch (severity)
		{
			using enum nvrhi::MessageSeverity;
		case Info           : RYU_LOG_INFO("{}", messageText);  break;
		case Warning        : RYU_LOG_WARN("{}", messageText);  break;
		case Error          : RYU_LOG_ERROR("{}", messageText); break;
		case Fatal          : RYU_LOG_FATAL("{}", messageText); break;
		}
	}
}

