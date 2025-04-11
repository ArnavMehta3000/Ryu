#pragma once
#include "GraphicsDX12/DX12Types.h"

namespace Ryu::Graphics::DX12
{
	class DX12Device;

	class DX12DeviceResource
	{
	protected:
		explicit DX12DeviceResource(const DX12Device* device)
			: m_device(device)
		{
		}

	public:
		const inline DX12Device* GetDevice() const { return m_device; }
		operator IDX12Device* () const;

	private:
		const DX12Device* m_device{ nullptr };
	};
}
