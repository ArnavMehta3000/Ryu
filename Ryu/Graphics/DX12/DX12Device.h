#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;
	class DX12DebugDevice;

	class DX12Device : public ComPtr<ID3D12Device6>
	{
	public:
		explicit DX12Device(InterfaceType* ptr);
		~DX12Device();

		static NODISCARD CreateResult<InterfaceType*> Create(const DXGIAdapter& adapter);
		
		DX12DebugDevice GetDebugDevice() const;
		void Release();
	};
}
