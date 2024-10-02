#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12CommandAllocator : public ComPtr<ID3D12CommandAllocator>
	{
	public:
		DX12CommandAllocator() = default;
		DX12CommandAllocator(InterfaceType* ptr) : ComPtr(ptr) {}
	};
}
