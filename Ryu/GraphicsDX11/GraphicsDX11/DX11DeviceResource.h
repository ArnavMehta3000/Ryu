#pragma once
#include "GraphicsDX11/DX11Types.h"

namespace Ryu::Graphics::DX11
{
	class DX11Device;

	class DX11DeviceResource
	{
	protected:
		explicit DX11DeviceResource(const DX11Device* device)
			: m_device(device)
		{
		}

	public:
		const inline DX11Device* GetDevice() const { return m_device; }
		operator IDX11Device* () const;

	private:
		const DX11Device* m_device{ nullptr };
	};
}
