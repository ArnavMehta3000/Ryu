#pragma once
#include "Common/Concepts.h"

namespace Ryu
{
	template <typename T>
	struct ITickable
	{
		ITickable()
		{
			static_assert(IsTickable<T>, "T must satisfy IsTickable to be used with ITickable.");
		}
	};
}
