#pragma once
#include "Graphics/DX12/DX12Texture.h"

namespace Ryu::Graphics::DX12
{
	class DX12DepthBuffer
	{
		RYU_DISABLE_COPY(DX12DepthBuffer);

	public:
		DX12DepthBuffer() = default;
		~DX12DepthBuffer();
		explicit DX12DepthBuffer(DX12TextureCreateDesc& desc);
		constexpr DX12DepthBuffer(DX12DepthBuffer&& other) noexcept;
		constexpr DX12DepthBuffer& operator=(DX12DepthBuffer&& other) noexcept;

		void Release();

		constexpr inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSV() const noexcept { return m_dsv.CPU; }
		constexpr ID3D12Resource* const GetResource() const noexcept { return m_texture.GetResource(); }
		constexpr DescriptorHandle GetSRV() const noexcept { return m_texture.GetSRV(); }

	private:
		DX12Texture m_texture;
		DescriptorHandle m_dsv;
	};
}
