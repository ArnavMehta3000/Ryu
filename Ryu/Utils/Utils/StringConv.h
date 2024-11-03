#pragma once
#include "Common/StandardTypes.h"
#include "Common/API.h"
#include <string>
#include <string_view>

namespace Ryu::Utils
{
	using namespace Common::StandardTypes;

	/**
	 * @brief Convert string to wide string
	 * @param str The string to convert
	 * @return The wide string
	 */
	RYU_API std::wstring ToWideStr(const std::string& str) noexcept;

	/**
	 * @brief Convert wide string to narrow string
	 * @param wstr The wide string
	 * @return The narrow string
	 */
	RYU_API std::string ToNarrowStr(const std::wstring& wstr) noexcept;

	/**
	 * @brief Convert bytes to string
	 * @param data Raw data
	 * @param size Size of raw data
	 * @return Data as string view
	 */
	RYU_API std::string_view StringFromBytes(const byte* data, size_t size);

	/**
	 * @brief Convert HRESULT to string
	 * @param hr The HRESULT
	 * @return The string representation of the HRESULT
	 */
	RYU_API std::string HRToString(long hr);
}
