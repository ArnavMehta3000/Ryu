#pragma once

namespace Ryu::Graphics
{
	class IGraphicsObject;
	class ISwapChain;
	class IDevice;

	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;

		virtual void InitializeResource(IGraphicsObject* obj) = 0;
		virtual ISwapChain* GetSwapChain() const = 0;
		virtual IDevice* GetDevice() const = 0;
	};
}
