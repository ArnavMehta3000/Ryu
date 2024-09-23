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
		~DX12Device();

		void Create(DXGIAdapter adapter);
		DX12DebugDevice GetDebugDevice() const;
		void Release();
	};
}
