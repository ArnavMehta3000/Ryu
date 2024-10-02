#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Resource : public ComPtr<ID3D12Resource>
	{
	public:
		DX12Resource() = default;
		DX12Resource(InterfaceType* ptr) : ComPtr(ptr) {}
	};
}
