#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DX12Device;

	class DX12DebugDevice : public ComPtr<ID3D12DebugDevice2>
	{
	public:
		explicit DX12DebugDevice(const DX12Device& device);
	};
}
