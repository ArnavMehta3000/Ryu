#pragma once
#include "Common/Concepts.h"
#include "Common/API.h"

namespace Ryu
{
	/**
	 * @brief Interface for tickable objects
	 * @details Tickable objects are objects that can be updated in the game loop. static_assert is used to ensure that the object satisfies the IsTickable concept
	 * @tparam T Type of object
	 */
	template <typename T>
	struct RYU_API ITickable
	{
		ITickable()
		{
			static_assert(IsTickable<T>, "T must satisfy IsTickable concept to be used with ITickable interface");
		}
	};
}
