#include "TomlParser.h"
#include <Core/ObjectMacros.h>
#include <sstream>

namespace Ryu
{
	TomlParser::ParseResult TomlParser::ReadFromFile(MAYBE_UNUSED std::string_view filePath)
	{
		try
		{
			m_table = toml::parse(filePath);
		}
		catch (const toml::parse_error& err)
		{
			std::stringstream ss;
			ss << err;
			return std::unexpected(ss.str());
		}

		return m_table;
	}
	
	TomlParser::ParseResult TomlParser::ReadFromMemory(const byte* data, size_t size)
	{
		return ReadFromMemory(StringFromBytes(data, size).data());  // From StringConv.h
	}
	
	TomlParser::ParseResult TomlParser::ReadFromMemory(const std::string_view& buffer)
	{
		try
		{
			m_table = toml::parse(buffer);
		}
		catch (const toml::parse_error& err)
		{
			std::stringstream ss;
			ss << err;
			return std::unexpected(ss.str());
		}

		return m_table;
	}
}
