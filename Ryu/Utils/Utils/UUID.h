#pragma once
#include "Utils/Random/Random.h"
#include <uuid_v4.h>
#include <string>

namespace Ryu::Utils
{
	/**
	 * @brief Universally Unique Identifier
	 */
	class RYU_API UUID
	{
	public:
		using Generator = UUIDv4::UUIDGenerator<std::mt19937_64>;
		using Type = UUIDv4::UUID;

		static Type Generate();
		static Type FromString(const std::string& str);
	};
}
