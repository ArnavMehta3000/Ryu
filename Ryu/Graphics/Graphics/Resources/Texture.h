#pragma once
#include "Graphics/Core/DeviceObject.h"
#include <optional>

namespace Ryu::Gfx
{
	struct TextureInitInfo
	{
		std::optional<DX12::Heap*>         Heap         = std::nullopt;
		std::optional<DX12::Resource*>     Resource     = std::nullopt;
		CD3DX12_RESOURCE_DESC*             ResourceDesc = nullptr;
		CD3DX12_SHADER_RESOURCE_VIEW_DESC  SRVDesc;
		D3D12_RESOURCE_ALLOCATION_INFO1    AllocationInfo;
		D3D12_RESOURCE_STATES              InitialState;
		CD3DX12_CLEAR_VALUE                ClearValue;
	};

	class Texture : public DeviceObject<Texture>
	{
		RYU_GFX_DEVICE_OBJ;
	public:
		static constexpr u32 MAX_MIPS = 14;

		RYU_GFX_NATIVE(m_resource)
		
		Texture() = default;
		explicit Texture(DeviceWeakPtr parent, const TextureInitInfo& info, DescriptorHeap& srvHeap);
		~Texture();

		inline DescriptorHandle GetSRV() const { return m_srv; }
		inline D3D12_RESOURCE_DESC1 GetDesc() const { return m_resource->GetDesc1(); }

	private:
		void OnConstruct(const TextureInitInfo& info, DescriptorHeap& srvHeap);
		void OnDestruct();

	private:
		ComPtr<DX12::Resource> m_resource;
		DescriptorHandle m_srv;
		DescriptorHeap* m_srvHeap = nullptr;
	};
}
