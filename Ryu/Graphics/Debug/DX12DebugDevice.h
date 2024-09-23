#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12DebugDevice : public ComPtr<ID3D12DebugDevice2>
	{
	public:
		explicit DX12DebugDevice(ID3D12Device6* device);
	};
}
