#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/Device.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(GFXCommandList);

	CommandList::CommandList(std::weak_ptr<Device> parent, CommandListType type)
		: DeviceObject(parent)
	{
		OnConstruct(type);
	}
	
	CommandList::~CommandList()
	{
		RYU_TODO("Reset command list here when we get to it");
	}

	void CommandList::ResetCommandList(DX12::CommandAllocator* allocator)
	{
		RYU_PROFILE_SCOPE();
		DX12::CommandAllocator* alloc = allocator ? allocator : m_cmdAllocator.Get();

		// Reset allocator and cmd list
		if (alloc && m_cmdList)
		{
			alloc->Reset();

			RYU_TODO("Set pipeline state here when we get to it");
			m_cmdList->Reset(alloc, nullptr);
			m_inFlight = true;
		}
	}

	void CommandList::CloseCommandList()
	{
		RYU_PROFILE_SCOPE();
		if (m_cmdList && m_inFlight)
		{
			m_cmdList->Close();
			m_inFlight = false;
		}
	}
	
	void CommandList::OnConstruct(CommandListType type)
	{
		RYU_PROFILE_SCOPE();
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			DXCallEx(device->CreateCommandAllocator(DX12::ToNative(type), IID_PPV_ARGS(&m_cmdAllocator)), device);
			DXCallEx(device->CreateCommandList(0, DX12::ToNative(type), m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)), device);
			
			// Command lists are created in the recording state close it instantly
			m_cmdList->Close();
			m_inFlight = false;
		}
		else
		{
			RYU_LOG_ERROR(LogGFXCommandList, "Trying to construct graphics command list ({}). Parent device is null", EnumToString(type));
		}
	}
}
