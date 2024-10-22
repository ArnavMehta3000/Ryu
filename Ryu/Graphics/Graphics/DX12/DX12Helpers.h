#pragma once
#include "Graphics/Common.h"

namespace Ryu::Graphics::DX12::Helpers
{
	constexpr inline D3D12_HEAP_PROPERTIES DefaultHeapProperties()
	{
		return D3D12_HEAP_PROPERTIES
		{
			.Type                 = D3D12_HEAP_TYPE_DEFAULT,
			.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask     = 0,
			.VisibleNodeMask      = 0
		};
	};

	ID3D12RootSignature* CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& desc);
}
