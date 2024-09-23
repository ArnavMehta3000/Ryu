#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;
	class DX12DebugDevice;

	class DX12Device : public ComPtr<ID3D12Device6>
	{
	public:
		DX12Device() = default;
		explicit DX12Device(DXGIAdapter adapter);
		~DX12Device();

		DX12DebugDevice GetDebugDevice() const;
		void Release();
	};
}
