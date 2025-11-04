#pragma once
#include "Graphics/Core/GfxDeviceChild.h"
#include <span>

namespace Ryu::Gfx
{
	struct GfxDescriptorHandle;

	class GfxCommandList : public GfxDeviceChild
	{
	public:
		GfxCommandList(GfxDevice* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name);
		virtual ~GfxCommandList() override = default;

		virtual void ReleaseObject() override;

		RYU_GFX_NATIVE(m_cmdList)

		void Begin(u32 frameIndex);
		void End();

		void SetViewports(std::span<const CD3DX12_VIEWPORT> viewports, std::span<const CD3DX12_RECT> scissors);
		void ResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier);
		void ResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers);
		void TransitionResource(DX12::Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		void SetRenderTarget(const GfxDescriptorHandle& rtv, const GfxDescriptorHandle& dsv);
		void SetTopology(D3D12_PRIMITIVE_TOPOLOGY topology);

	private:
		ComPtr<DX12::GraphicsCommandList> m_cmdList;
		ComFrameArray<DX12::CommandAllocator> m_cmdAllocators;
		D3D12_COMMAND_LIST_TYPE m_type;
	};
}
