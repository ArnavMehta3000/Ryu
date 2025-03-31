#include "IDevice.h"
#include "Common/StandardTypes.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/Config.h"
#include "GraphicsRHI/IRenderer.h"
#include "GraphicsRHI/Utils/Logging.h"
#include <dxgidebug.h>
#include <libassert/assert.hpp>

namespace Ryu::Graphics
{
	void IDevice::InitializeResource(IGraphicsObject* obj) const
	{
		RYU_PROFILE_SCOPE();
		if (auto renderer = GetRendererInterface())
		{
			renderer->InitializeResource(obj);
		}
		else
		{
			LOG_WARN(Internal::GraphicsDebugLog, "Renderer is not initialized on device. Failing to initialize resource.");
		}
	}
	
	bool IDevice::CheckFeatureSupport(DXGI_FEATURE feature)
	{
		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory not initialized");

		BOOL result{ 0 };
		DXCall(m_dxgiFactory->CheckFeatureSupport(feature, &result, sizeof(BOOL)));

		return result == TRUE;
	}

	bool IDevice::InitializeDXGI(bool enableDebugLayer)
	{
		RYU_PROFILE_SCOPE();
		if (enableDebugLayer)
		{
			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			}
		}

		// Create DXGI Factory
		DXCall(::CreateDXGIFactory2(enableDebugLayer ?
			DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&m_dxgiFactory)));

		// Get adapter with highest performance
		for (u32 i = 0; m_dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&m_adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC3 desc{};
			DXCall(m_adapter->GetDesc3(&desc));

			// Skip software adapter (WARP)
			if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
			{
				continue;
			}
			
			return true;
		}

		// If no hardware adapter found, try WARP
		DXCall(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter)));

		if (!m_adapter)
		{
			return false;
		}

		return true;
	}
	
	void IDevice::ShutdownDXGI()
	{
		RYU_PROFILE_SCOPE();
		m_adapter.Reset();
		m_dxgiFactory.Reset();

		ComPtr<IDXGIDebug1> dxgiDebug;
		if (GraphicsConfig::Get().EnableDebugLayer &&
			SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiDebug.GetAddressOf()))))
		{
			LOG_DEBUG(Internal::GraphicsDebugLog, "~~~DXGI Report Live Objects~~~");

			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(
				DXGI_DEBUG_RLO_SUMMARY |
				DXGI_DEBUG_RLO_DETAIL |
				DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
}
