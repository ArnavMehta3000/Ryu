#include "DX12Device.h"
#include "Profiling/Profiling.h"
#include "GraphicsRHI/GraphicsConfig.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsDX12/DX12SwapChain.h"
#include "GraphicsDX12/DX12CommandList.h"
#include <libassert/assert.hpp>

namespace Ryu::Graphics::DX12
{
	DX12DeviceResource::operator IDX12Device* () const { return *m_device; }

	DX12Device::DX12Device()
	{
		RYU_PROFILE_SCOPE();
		DEBUG_ASSERT((GraphicsConfig::Get().GraphicsAPI.Get() == API::DirectX12),
			"Graphics API must be DirectX12!");

		InitDevice();
	}

	DX12Device::~DX12Device()
	{
		RYU_PROFILE_SCOPE();
		ShutdownDXGI();

		m_cmdQueue.Reset();
		
		if (GraphicsConfig::Get().EnableDebugLayer)
		{
			ReportLiveObjects(true);
		}
		else
		{
			m_device.Reset();
		}
	}
	
	DX12::IDX12Fence* DX12Device::CreateFence(u64 initialValue, D3D12_FENCE_FLAGS flags) const
	{
		DX12::IDX12Fence* fence = nullptr;
		DXCall(m_device->CreateFence(initialValue, flags, IID_PPV_ARGS(&fence)));

		return fence;
	}

	DX12::IDX12DescriptorHeap* DX12Device::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count)
	{
		DX12::IDX12DescriptorHeap* descHeap = nullptr;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = count;
		desc.Type = type;

		DXCall(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descHeap)));

		return descHeap;
	}

	void DX12Device::ReportLiveObjects(bool releaseBeforeReporting)
	{
		RYU_PROFILE_SCOPE();
		ComPtr<ID3D12DebugDevice> debugDevice;
		if (SUCCEEDED(m_device.As(&debugDevice)))
		{
			if (releaseBeforeReporting)
			{
				m_device.Reset();  // Release the device after acquring its debug interface
			}

			LOG_DEBUG(Internal::GraphicsDebugLog, "~~~DX12 Report Live Objects~~~");
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY | D3D12_RLDO_IGNORE_INTERNAL);
		}
	}
	
	IDevice::CreateSwapChainResult DX12Device::CreateSwapChain(const SwapChainDesc& desc) const
	{
		RYU_PROFILE_SCOPE();
		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");
		DEBUG_ASSERT(m_cmdQueue, "Adapter is not initialized!");

		if (!m_dxgiFactory || !m_adapter || !m_cmdQueue)
		{
			return MakeResultError{"Failed to create swap chain"};
		}

		return std::make_unique<DX12SwapChain>(this, desc);
	}

	IDevice::CreateCommandListResult DX12Device::CreateCommandList(const CommandListDesc& desc) const
	{
		RYU_PROFILE_SCOPE();
		DEBUG_ASSERT(m_device, "DX12Device is not initialized!");

		return std::make_unique<DX12CommandList>(this, desc);
	}

	void DX12Device::ExecuteCommandList(const ICommandList* commandList) const
	{
		RYU_PROFILE_SCOPE();
		if (DX12CommandList::NativeType* cmdList = RYU_GET_GFX_NATIVE_TYPE(DX12CommandList::NativeType, commandList))
		{
			DX12::IDX12CommandList* const cmdLists[] = { cmdList };
			m_cmdQueue->ExecuteCommandLists(1, cmdLists);

			// Signal and wait for execution if immediate sync is needed
			if (auto dx12CmdList = dynamic_cast<const DX12CommandList*>(commandList))
			{
				dx12CmdList->Signal();
			}
		}
	}
	
	void DX12Device::InitDevice()
	{
		RYU_PROFILE_SCOPE();
		static constexpr std::array<D3D_FEATURE_LEVEL, 4> featureLevels =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		const GraphicsConfig& config = GraphicsConfig::Get();

		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12Debug6> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				LOG_DEBUG(Internal::GraphicsDebugLog, "Graphics debug layer enabled");

				if (config.EnableGPUBasedValidation)
				{
					LOG_DEBUG(Internal::GraphicsDebugLog, "Enabling GPU-based validation");
					debugController->SetEnableGPUBasedValidation(TRUE);
				}
			}
		}

		if (!InitializeDXGI(config.EnableDebugLayer))
		{
			LOG_ERROR(Internal::GraphicsDebugLog, "Failed to initialize DXGI!");
			return;
		}

		DEBUG_ASSERT(m_dxgiFactory, "DXGI factory is not initialized!");
		DEBUG_ASSERT(m_adapter, "Adapter is not initialized!");

		// Create device
		for (auto level : featureLevels)
		{
			if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), level, IID_PPV_ARGS(&m_device))))
			{
				break;
			}
		}

		DEBUG_ASSERT(m_device, "Failed to create DX12 device!");
			
		CreateCommandQueue();
		CreateDescriptorHeaps();

		// Set up debug messages
		if (config.EnableDebugLayer)
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(m_device.As(&infoQueue)))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

				D3D12_MESSAGE_ID hide[] = 
				{
					D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
					D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
				};

				D3D12_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
				filter.DenyList.pIDList = hide;
				infoQueue->AddStorageFilterEntries(&filter);
			}
		}

		DX12_NAME_OBJECT(m_device.Get(), L"DX12 Main Device");
		DX12_NAME_OBJECT(m_cmdQueue.Get(), L"DX12 Command Queue");
	}
	
	void DX12Device::CreateCommandQueue()
	{
		RYU_PROFILE_SCOPE();
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.NodeMask = 0;

		DXCall(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue)));
	}
	
	void DX12Device::CreateDescriptorHeaps()
	{
		RYU_PROFILE_SCOPE();
	}
}
