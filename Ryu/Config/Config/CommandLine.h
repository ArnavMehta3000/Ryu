#pragma once
#include "Common/Common.h"
#include <variant>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace Ryu::Config
{
	/**
	 * @brief Class to parse and store command line arguments
	 */
	class CommandLine
	{
	public:
		/**
		 * @brief Variant type for command line arguments
		 */
		using ArgumentValue = std::variant<bool, i32, std::string>;
		
		CommandLine() = default;
		/**
		 * @brief Constructs a new command line object from a command line string
		 * @param cmdLine Input command line (wide) string
		 */
		explicit CommandLine(std::wstring_view cmdLine);

		/**
		 * @brief Checks if a command line argument exists
		 * @param key Argument key
		 * @return True if the argument exists, false otherwise
		 */
		RYU_API bool GetBool(std::string_view key) const;

		/**
		 * @brief Returns the `i32` value of a command line argument
		 * @param key Argument key
		 * @return The value of the argument or std::nullopt
		 */
		RYU_API std::optional<i32> GetInt(std::string_view key) const;

		/**
		 * @brief Returns the `std::string` value of a command line argument
		 * @param key Argument key
		 * @return The value of the argument or std::nullopt
		 */
		RYU_API std::optional<std::string> GetString(std::string_view key) const;

	private:
		void ParseArguments();

	private:
		std::vector<std::string> m_arguments;
		std::unordered_map<std::string, ArgumentValue> m_argumentsMap;
	};
}
