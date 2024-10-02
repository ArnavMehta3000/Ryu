#pragma once
#include <Graphics/Common.h>

namespace Ryu::Graphics
{
	class DXGIAdapter;
	class DX12DebugDevice;
	struct DescriptorSizes;

	class DX12Device : public ComPtr<ID3D12Device6>
	{
	public:
		DX12Device(InterfaceType* ptr) : ComPtr(ptr) {}
		DX12Device() = default;
		~DX12Device() = default;

		static NODISCARD CreateResult<DX12Device> Create(const DXGIAdapter& adapter);
		
		DX12DebugDevice GetDebugDevice() const;
		DescriptorSizes GetDescriptorSizes() const;
		void Release();
	};
}
