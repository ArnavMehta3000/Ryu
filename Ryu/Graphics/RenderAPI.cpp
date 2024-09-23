#include "RenderAPI.h"
#include <Core/Utils/StringConv.h>
#include <Graphics/DXGI/DXGIFactory.h>
#include <Graphics/DXGI/DXGIAdapter.h>

#include <Graphics/Debug/DX12DebugDevice.h>

namespace Ryu::Graphics
{
	RenderAPI::RenderAPI()
		: m_device(nullptr)
	{
	}
	
	RenderAPI::~RenderAPI()
	{
		m_device.reset();
	}

	bool RenderAPI::Initialize(HWND hWnd)
	{
		RYU_GFX_INFO("Initializing RenderAPI");

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		{
			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);
			std::string name = ToNarrowStr(desc.Description);
			RYU_GFX_DEBUG("Selected adapter: {} | Vendor Id: {}", name, desc.VendorId);
		}

		m_device = std::make_unique<DX12Device>(adapter);
		m_commandQueue.Create(m_device->Get(), D3D12_COMMAND_QUEUE_PRIORITY_HIGH);

		RYU_GFX_TRACE("Finished initializing RenderAPI");
		return true;
	}

	void RenderAPI::Shutdown()
	{
		m_commandQueue.Release();
		m_device->Release();
	}
}
