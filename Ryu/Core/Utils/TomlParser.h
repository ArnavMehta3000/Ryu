#pragma once
#include <Core/Utils/StringConv.h>
#include <toml++/toml.hpp>
#include <expected>

namespace Ryu
{
	class RYU_API TomlParser
	{
	public:
		using Table = toml::table;
		using ParseResult = std::expected<std::reference_wrapper<Table>, std::string>;
		ParseResult ReadFromFile(std::string_view filePath);
		ParseResult ReadFromMemory(const byte* data, size_t size);
		ParseResult ReadFromMemory(const std::string_view& buffer);

		inline Table& GetTable() { return m_table; }

	private:
		Table m_table;
	};
}