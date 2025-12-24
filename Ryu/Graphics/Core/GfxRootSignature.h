#pragma once
#include "Graphics/Core/GfxDeviceChild.h"

struct IDxcBlob;

namespace Ryu::Gfx
{
	class RootSignature : public DeviceChild
	{
	public:
		RootSignature(Device* parent, const CD3DX12_ROOT_SIGNATURE_DESC& desc, std::string_view name);
		RootSignature(Device* parent, IDxcBlob* blob, std::string_view name);

		virtual ~RootSignature() = default;

		virtual inline void ReleaseObject() override { ComRelease(m_rootSignature); }

		RYU_GFX_NATIVE(m_rootSignature)

	private:
		void CreateInternal(void* data, u64 size, std::string_view name);

	private:
		ComPtr<DX12::RootSignature> m_rootSignature;
	};
}
