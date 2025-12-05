module;
#include <Windows.h>
#include <comdef.h>
#include <string>
#include <string_view>

export module Ryu.Core.Utils:StringConv;

export namespace Ryu::Utils
{
	/**
	 * @brief Convert string to wide string
	 * @param str The string to convert
	 * @return The wide string
	 */
	std::wstring ToWideStr(const std::string& str) noexcept
    {
		int cch = ::MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), nullptr, 0);

		std::wstring wideStr;
		wideStr.resize(cch);
		::MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), wideStr.data(), cch);

		return wideStr;
	}

	/**
	 * @brief Convert wide string to narrow string
	 * @param wstr The wide string
	 * @return The narrow string
	 */
	std::string ToNarrowStr(const std::wstring& wstr) noexcept
    {
		int cch = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);

		std::string narrowStr;
		narrowStr.resize(cch);
		::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), &narrowStr[0], cch, nullptr, nullptr);

		return narrowStr;
	}

	/**
	 * @brief Convert bytes to string
	 * @param data Raw data
	 * @param size Size of raw data
	 * @return Data as string view
	 */
	std::string_view StringFromBytes(const byte* data, size_t size)
    {
		return std::string_view(reinterpret_cast<const char*>(data), size);
	}

	/**
	 * @brief Convert HRESULT to string
	 * @param hr The HRESULT
	 * @return The string representation of the HRESULT
	 */
	std::string HRToString(long hr)
    {
		HRESULT hres = static_cast<HRESULT>(hr);
		_com_error err(hres);
		return ToNarrowStr(err.ErrorMessage());
	}
}