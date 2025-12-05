module;
#include <cstdint>

export module Ryu.Common:Types;

/**
	* @brief Signed 8-bit integer
	*/
export using i8  = std::int8_t;

/**
	* @brief Signed 16-bit integer
	*/
export using i16 = std::int16_t;

/**
	* @brief Signed 32-bit integer
	*/
export using i32 = std::int32_t;

/**
	* @brief Signed 64-bit integer
	*/
export using i64 = std::int64_t;


/**
	* @brief Unsigned 8-bit integer
	*/
export using u8  = std::uint8_t;

/**
	* @brief Unsigned 16-bit integer
	*/
export using u16 = std::uint16_t;

/**
	* @brief Unsigned 32-bit integer
	*/
export using u32 = std::uint32_t;

/**
	* @brief Unsigned 64-bit integer
	*/
export using u64 = std::uint64_t;


/**
	* @brief 32-bit floating point
	*/
export using f32  = float;

/**
	* @brief 64-bit floating point
	*/
export using f64  = double;

/**
	* @brief 16-bit character
	*/
export using char16 = char16_t;

/**
	* @brief 32-bit character
	*/
export using char32 = char32_t;

/**
	* @brief 8-bit byte
	*/
export using byte = std::uint8_t;
