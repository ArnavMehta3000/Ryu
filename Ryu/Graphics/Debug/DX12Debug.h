#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Debug : public ComPtr<ID3D12Debug5>
	{
	public:
		DX12Debug() = default;
		explicit DX12Debug(InterfaceType* ptr);
		static CreateResult<DX12Debug> Create();
	};
}
