module;
#include <uuid_v4.h>
#include <string>

export module Ryu.Core.Utils:UUID;
import :Random;

namespace Ryu::Utils
{
	/**
	 * @brief Universally Unique Identifier
	 */
	export class UUID
	{
	public:
		using Generator = UUIDv4::UUIDGenerator<std::mt19937_64>;
		using Type = UUIDv4::UUID;

		// Generate UUID
		static Type Generate();

		// Generate UUID (16) bytes string
		static std::string GenerateBytes();

		// Generate UUID (32) bytes pretty string
		static std::string GenerateString();

		// Create UUID from pretty string (36 bytes)
		static Type FromString(const std::string& str);

		// Create UUID from a byte string (16 bytes)
		static Type FromBytes(const std::string& str);

		// Converts a UUID byte string to UUID pretty string
		static std::string FromBytesToPrettyString(const std::string& str);
		
		// Converts a UUID pretty string to UUID byte string
		static std::string FromPrettyStringToBytes(const std::string& str);
	};
}
