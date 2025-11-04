#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxDescriptorHeap.h"

namespace Ryu::Gfx
{
	CommandList::CommandList(Device* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name)
		: DeviceChild(parent)
		, m_type(type)
	{
		DX12::Device* device = GetDevice()->GetNativeDevice();

		for (i32 i = 0; i < m_cmdAllocators.size(); i++)
		{
			const std::string allocatorName = name.data() + fmt::format(" Allocator {}", i);

			DXCall(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_cmdAllocators[i])));
			DX12::SetObjectName(m_cmdAllocators[i].Get(), allocatorName.data());
		}

		
		DXCall(device->CreateCommandList(0, type, m_cmdAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_cmdList)));
		DX12::SetObjectName(m_cmdList.Get(), name.data());

		m_cmdList->Close();
	}

	void CommandList::ReleaseObject()
	{
		ComRelease(m_cmdList);

		for (auto& allocator : m_cmdAllocators)
		{
			ComRelease(allocator);
		}
	}
	
	void CommandList::Begin(u32 frameIndex)
	{
		DXCall(m_cmdAllocators[frameIndex]->Reset());
		DXCall(m_cmdList->Reset(m_cmdAllocators[frameIndex].Get(), nullptr));
	}

	void CommandList::End()
	{
		DXCall(m_cmdList->Close());
	}
	
	void CommandList::SetViewports(std::span<const CD3DX12_VIEWPORT> viewports, std::span<const CD3DX12_RECT> scissors)
	{
		m_cmdList->RSSetViewports(u32(viewports.size()), viewports.data());
		m_cmdList->RSSetScissorRects(u32(scissors.size()), scissors.data());
	}

	void CommandList::ResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier)
	{
		m_cmdList->ResourceBarrier(1, &barrier);
	}
	
	void CommandList::ResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers)
	{
		m_cmdList->ResourceBarrier(u32(barriers.size()), barriers.data());
	}

	void CommandList::TransitionResource(DX12::Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);
		ResourceBarrier(barrier);
	}

	void CommandList::SetRenderTarget(const DescriptorHandle& rtv, const DescriptorHandle& dsv)
	{
		m_cmdList->OMSetRenderTargets(1, &rtv.CPU, FALSE, dsv.IsValid() ? &dsv.CPU : nullptr);
	}
	
	void CommandList::SetTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		m_cmdList->IASetPrimitiveTopology(topology);
	}
}
