#pragma once
#include "Graphics/Core/GfxDeviceChild.h"
#include <span>

namespace Ryu::Gfx
{
	struct DescriptorHandle;
	class PipelineState;
	class RootSignature;
	class Buffer;

	class CommandList : public DeviceChild
	{
	public:
		CommandList(Device* parent, D3D12_COMMAND_LIST_TYPE type, std::string_view name);
		virtual ~CommandList() override = default;

		virtual void ReleaseObject() override;

		RYU_GFX_NATIVE(m_cmdList)

		void Begin(u32 frameIndex, PipelineState* pipelineState);
		void End();

		void SetViewports(std::span<const CD3DX12_VIEWPORT> viewports, std::span<const CD3DX12_RECT> scissors) const;
		void SetRenderTarget(const DescriptorHandle& rtv, const DescriptorHandle& dsv) const;
		void SetVertexBuffer(u32 slot, const Buffer& buffer) const;
		void SetTopology(D3D12_PRIMITIVE_TOPOLOGY topology) const;
		void SetGraphicsRootSignature(const RootSignature& rootSignature) const;
		
		void ResourceBarrier(const CD3DX12_RESOURCE_BARRIER& barrier) const;
		void ResourceBarriers(std::span<const CD3DX12_RESOURCE_BARRIER> barriers) const;
		void TransitionResource(DX12::Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const;

		void DrawInstanced(u32 vertexCountPerInstance, u32 instanceCount, u32 startVertexLocation, u32 startInstanceLocation) const;

	private:
		D3D12_COMMAND_LIST_TYPE               m_type;
		ComPtr<DX12::GraphicsCommandList>     m_cmdList;
		ComFrameArray<DX12::CommandAllocator> m_cmdAllocators;
	};
}
