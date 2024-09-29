#pragma once
#include <CoreDLLDefines.h>
#include <Core/StandardTypes.h>
#include <string>
#include <string_view>

namespace Ryu
{
	std::wstring RYU_API ToWideStr(const std::string& str) noexcept;
	std::string RYU_API ToNarrowStr(const std::wstring& wstr) noexcept;
	std::string_view RYU_API StringFromBytes(const byte* data, size_t size);
	std::string RYU_API HRToString(long hr);
}
