#pragma once
#include <CoreDLLDefines.h>

namespace Ryu::Input
{
	struct InputCallbacks;

	class RYU_API IInputInterface
	{
	public:
		virtual ~IInputInterface() = default;
		virtual void AddInputCallbacks(const InputCallbacks& callbacks) = 0;
	};
}
