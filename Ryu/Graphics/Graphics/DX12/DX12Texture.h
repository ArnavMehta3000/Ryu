#pragma once
#include "Graphics/DX12/DX12Resources.h"

namespace Ryu::Graphics::DX12
{
	struct DX12TextureCreateDesc
	{
		ID3D12Heap1* Heap{ nullptr };
		ID3D12Resource* Resource{ nullptr };
		D3D12_SHADER_RESOURCE_VIEW_DESC* SRVDesc{ nullptr };
		D3D12_RESOURCE_DESC* ResourceDesc{ nullptr };
		D3D12_RESOURCE_ALLOCATION_INFO1 AllocationInfo{};
		D3D12_CLEAR_VALUE ClearValue{};
		D3D12_RESOURCE_STATES InitialState{};
	};

	class DX12Texture
	{
		RYU_DISABLE_COPY(DX12Texture);

	public:
		DX12Texture() = default;
		~DX12Texture();
		explicit DX12Texture(const DX12TextureCreateDesc& desc);
		constexpr DX12Texture(DX12Texture&& other) noexcept;
		constexpr DX12Texture& operator=(DX12Texture&& other) noexcept;

		void Release();

		constexpr inline ID3D12Resource* GetResource() const { return m_resource; }
		constexpr inline DescriptorHandle GetSRV() const { return m_srv; }

	private:
		constexpr void Reset();
		constexpr void Move(DX12Texture& other);

	public:
		static constexpr u32 MAX_MIPS = 14;  // 2^14 = 16k resolution

	private:
		ID3D12Resource* m_resource;
		DescriptorHandle m_srv;
	};
}
