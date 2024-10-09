#pragma once
#include "Utils/Random/Random.h"
#include <uuid_v4.h>

namespace Ryu::Utils
{
	class UUID
	{
	public:
		using Generator = UUIDv4::UUIDGenerator<std::mt19937_64>;
		using Type = UUIDv4::UUID;

	public:
		static Type Generate();
	};
}
