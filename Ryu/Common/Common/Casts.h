#pragma once
#include <concepts>
#include <type_traits>
#include <cassert>

namespace Ryu
{
	// Static cast
	template <typename To, typename From>
	[[nodiscard]] constexpr To SCast(From&& value) noexcept(noexcept(static_cast<To>(std::forward<From>(value))))
	{
		return static_cast<To>(std::forward<From>(value));
	}

	// Dynamic cast
	template <typename To, typename From>
	[[nodiscard]] constexpr To DCast(From&& value) noexcept(std::is_pointer_v<std::remove_reference_t<To>>)
	{
		return dynamic_cast<To>(std::forward<From>(value));
	}

	// Const cast
	template <typename To, typename From>
	[[nodiscard]] constexpr To CCast(From&& value) noexcept 
	{
		return const_cast<To>(std::forward<From>(value));
	}

	// Reinterpret cast
	template <typename To, typename From>
	[[nodiscard]] constexpr To RCast(From&& value) noexcept 
	{
		return reinterpret_cast<To>(std::forward<From>(value));
	}

	// Bit cast
	template <typename To, typename From>
		requires(sizeof(To) == sizeof(From) 
		&& std::is_trivially_copyable_v<From>
		&& std::is_trivially_copyable_v<To>)
		[[nodiscard]] constexpr To BCast(const From& value) noexcept
	{
		return __builtin_bit_cast(To, value);
	}

	// Narrowing cast
	template <typename To, typename From>
		requires(std::is_arithmetic_v<To>&& std::is_arithmetic_v<From>)
	[[nodiscard]] constexpr To NCast(From value) noexcept
	{
#if !defined(RYU_BUILD_DEBUG)
		return static_cast<To>(value);
#else
		const To result = static_cast<To>(value);
		// Check for precision loss
		const From back = static_cast<From>(result);

		// For floating point, allow small epsilon difference
		if constexpr (std::is_floating_point_v<From> || std::is_floating_point_v<To>)
		{
			if constexpr (std::is_floating_point_v<From> && std::is_floating_point_v<To>)
			{
				// Float to float conversion
				[[maybe_unused]] const bool valid = (value == back)
					|| (std::isnan(value) && std::isnan(back))
					|| (std::isinf(value) && std::isinf(back) && ((value > 0) == (back > 0)));
				assert(valid && "Narrowing cast would lose precision");
			}
			else
			{
				// Integer to/from float
				[[maybe_unused]] const bool valid = (back == value);
				assert(valid && "Narrowing cast would lose precision");
			}
		}
		else {
			// Integer to integer
			[[maybe_unused]] const bool valid = (back == value)
				&& ((value < From{}) == (result < To{})); // Check sign preservation
			assert(valid && "Narrowing cast would lose value or change sign");
		}

		return result;
#endif
	}

	template <typename To, typename From>
	[[nodiscard]] constexpr To& PCast(From& value) noexcept 
	{
		return *std::launder(reinterpret_cast<To*>(&value));
	}

	template <typename To, typename From>
	[[nodiscard]] constexpr const To& PunCast(const From& value) noexcept
	{
		return *std::launder(reinterpret_cast<const To*>(&value));
	}
}
