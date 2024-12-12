#include "IDevice.h"
#include "Graphics/DX11/DX11Device.h"

namespace Ryu::Graphics
{
	IDevice::CreateDeviceResult IDevice::Create(const DeviceCreateDesc& desc)
	{
		switch (desc.GraphicsAPI)
		{
		case API::DirectX11:
			return std::make_unique<DX11Device>(desc);
		}

		return CreateDeviceResult::unexpected_type("Unsupported graphics API");
	}
}
