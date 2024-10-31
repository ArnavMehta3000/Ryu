#pragma once
#include <cstdint>

/**
 * @brief Namespace for standard types
 * @note All standard types are defined in the Ryu::Common::StandardTypes namespace
 */
namespace Ryu::Common::StandardTypes
{
	/**
	 * @brief Signed 8-bit integer
	 */
	using i8  = std::int8_t;

	/**
	 * @brief Signed 16-bit integer
	 */
	using i16 = std::int16_t;

	/**
	 * @brief Signed 32-bit integer
	 */
	using i32 = std::int32_t;

	/**
	 * @brief Signed 64-bit integer
	 */
	using i64 = std::int64_t;


	/**
	 * @brief Unsigned 8-bit integer
	 */
	using u8  = std::uint8_t;

	/**
	 * @brief Unsigned 16-bit integer
	 */
	using u16 = std::uint16_t;

	/**
	 * @brief Unsigned 32-bit integer
	 */
	using u32 = std::uint32_t;

/**
	 * @brief Unsigned 64-bit integer
	 */
	using u64 = std::uint64_t;


	/**
	 * @brief 32-bit floating point
	 */
	using f32  = float;

	/**
	 * @brief 64-bit floating point
	 */
	using f64  = double;


	/**
	 * @brief 8-bit byte
	 */
	using byte = std::uint8_t;
}
