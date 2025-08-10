#include "Graphics/Core/CommandList.h"
#include "Graphics/Core/Device.h"
#include "Graphics/Core/CommandAllocator.h"
#include "Graphics/Core/PipelineState.h"
#include "Logging/Logger.h"
#include <format>

namespace Ryu::Gfx
{
	RYU_LOG_DECLARE_CATEGORY(GfxCommandList);

	CommandList::CommandList(DeviceWeakPtr parent, CommandAllocator& allocator, PipelineState* pso, CommandListType type)
		: DeviceObject(parent)
		, m_state(State::Closed)
	{
		OnConstruct(allocator, pso, type);
	}

	CommandList::~CommandList()
	{
		OnDestruct();
	}

	void CommandList::Reset(CommandAllocator& allocator, PipelineState* pso) const
	{
		if (m_state == State::Recording)
		{
			RYU_LOG_WARN(LogGfxCommandList, "Resetting command list while recording - auto-closing first");
			Close();
		}

		DXCall(m_cmdList->Reset(allocator.Get(), pso ? *pso : nullptr));
		m_state = State::Recording;
	}

	void CommandList::Close() const
	{
		if (m_state == State::Recording)
		{
			DXCall(m_cmdList->Close());
			m_state = State::Closed;
		}
	}

	void CommandList::OnConstruct(CommandAllocator& allocator, PipelineState* pso, CommandListType type)
	{
		if (auto parent = GetParent())
		{
			DX12::Device* const device = parent->GetDevice();

			DXCallEx(device->CreateCommandList(
				0,
				DX12::ToNative(type),
				allocator,
				pso ? *pso : nullptr,
				IID_PPV_ARGS(&m_cmdList)),
			device);

			m_cmdList->Close();
			SetName(std::format("Command List ({})", EnumToString(type)).c_str());
		}
	}

	void CommandList::OnDestruct()
	{
		if (m_cmdList)
		{
			m_cmdList.Reset();
		}
	}
}
