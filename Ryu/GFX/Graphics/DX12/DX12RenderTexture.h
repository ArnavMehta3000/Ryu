#pragma once
#include "Graphics/DX12/DX12Texture.h"

namespace Ryu::Graphics::DX12
{
	class DX12Rendertexture
	{
		RYU_DISABLE_COPY(DX12Rendertexture);

	public:
		DX12Rendertexture() = default;
		~DX12Rendertexture();
		explicit DX12Rendertexture(const DX12TextureCreateDesc& desc);
		constexpr DX12Rendertexture(DX12Rendertexture&& other) noexcept;
		constexpr DX12Rendertexture& operator=(DX12Rendertexture&& other) noexcept;

		void Release();

		constexpr ID3D12Resource* const GetResource() const noexcept { return m_texture.GetResource(); }
		constexpr inline u32 GetMipCount() const noexcept{ return m_mipCount; }
		constexpr inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTV(u32 mipIndex) const { return m_rtv.at(mipIndex).CPU; }
		constexpr DescriptorHandle GetSRV() const noexcept { return m_texture.GetSRV(); }

	private:
		constexpr void Reset();
		constexpr void Move(DX12Rendertexture& other);

	private:
		DX12Texture m_texture;
		std::array<DescriptorHandle, DX12Texture::MAX_MIPS> m_rtv;
		u32 m_mipCount;
	};
}
