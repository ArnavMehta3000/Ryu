#pragma once
#include "Common/StandardTypes.h"
#include <string>
#include <string_view>

namespace Ryu::Utils
{
	using namespace Common::StandardTypes;

	std::wstring ToWideStr(const std::string& str) noexcept;
	std::string ToNarrowStr(const std::wstring& wstr) noexcept;
	std::string_view StringFromBytes(const byte* data, size_t size);
	std::string HRToString(long hr);
}
