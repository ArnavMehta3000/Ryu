module;
#include <string>
#include <string_view>

export module Ryu.Core.Utils:StringConv;
import Ryu.Core.Common;

export namespace Ryu::Utils
{
	/**
	 * @brief Convert string to wide string
	 * @param str The string to convert
	 * @return The wide string
	 */
	std::wstring ToWideStr(const std::string& str) noexcept;

	/**
	 * @brief Convert wide string to narrow string
	 * @param wstr The wide string
	 * @return The narrow string
	 */
	std::string ToNarrowStr(const std::wstring& wstr) noexcept;

	/**
	 * @brief Convert bytes to string
	 * @param data Raw data
	 * @param size Size of raw data
	 * @return Data as string view
	 */
	std::string_view StringFromBytes(const byte* data, size_t size);

	/**
	 * @brief Convert HRESULT to string
	 * @param hr The HRESULT
	 * @return The string representation of the HRESULT
	 */
	std::string HRToString(long hr);
}
