#pragma once

namespace Ryu::Graphics
{
	class ICommandList;
	class IRenderTarget;
	class ITexture2D;

	/**
	* @brief Interface for a render pass
	* @detail Non-resource owning class that is used to begin and end a render pass
	*/
	class IRenderPass
	{
	public:
		virtual void Begin(ICommandList* commandList) = 0;
		virtual void End(ICommandList* commandList) = 0;
	};
}
