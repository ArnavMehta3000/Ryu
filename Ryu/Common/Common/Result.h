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

	/**
	 * @brief Helper struct for creating a result with an error message
	 */
	struct MakeResultError
	{
		template <typename T>
		operator Result<T>() const 
		{
			return std::unexpected(Message);
		}

		operator VoidResult() const {
			return std::unexpected(Message);
		}

		std::string Message;
	};
}
