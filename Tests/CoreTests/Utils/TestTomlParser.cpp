#include "../../../Ryu/Core/Utils/TomlParser.h"
#include <sstream>
#include <string>
#include <string_view>
using namespace std::string_view_literals;

// Test to check if the TomlParser works
int main()
{
	// From Toml++ website
	static constexpr std::string_view data = R"(
		[library]
		name = "toml++"
		authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
		cpp = 17
	)"sv;

	Ryu::TomlParser parser;
	Ryu::TomlParser::ParseResult result = parser.ReadFromMemory(data);
	
	return result.has_value() ? 0 : 1;
}