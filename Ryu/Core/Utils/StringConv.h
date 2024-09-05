#pragma once
#include <string>

namespace Ryu
{
	std::wstring ToWideStr(const std::string& str) noexcept;
	std::string ToNarrowStr(const std::wstring& wstr) noexcept;
}