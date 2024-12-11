#pragma once
#include <expected>
#include <string>

namespace Ryu
{
	/**
	 * @brief Generic result type
	 * @details A result type is a type that can hold either a value or an error message
	 * @tparam ReturnType The type of the return value
	 */
	template <typename ReturnType>
	using Result = std::expected<ReturnType, std::string>;

	/**
	 * @brief Void result type
	 */
	using VoidResult = std::expected<void, std::string>;
}
