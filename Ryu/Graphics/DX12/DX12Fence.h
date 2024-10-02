#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Fence : public ComPtr<ID3D12Fence>
	{
	public:
		DX12Fence() = default;
		DX12Fence(InterfaceType* ptr) : ComPtr(ptr) {}
	};
}
