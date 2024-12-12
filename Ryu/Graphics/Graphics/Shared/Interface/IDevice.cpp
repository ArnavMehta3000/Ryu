#include "IDevice.h"
#include "Graphics/DX11/DX11Device.h"
#include <format>

namespace Ryu::Graphics
{
	IDevice::CreateDeviceResult IDevice::Create(const DeviceCreateDesc& desc)
	{
		switch (desc.GraphicsAPI)
		{
		case API::DirectX11:
			return std::make_unique<DX11Device>(desc);
		}

		return CreateDeviceResult::unexpected_type(std::format(
			"Unsupported graphics device API - {}", EnumToString(desc.GraphicsAPI)));
	}
}
