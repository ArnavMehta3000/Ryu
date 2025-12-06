module;
#include <Windows.h>
#include <comdef.h>
#include <string>
#include <string_view>

module Ryu.Core.Utils;
import :StringConv;

namespace Ryu::Utils
{
	std::wstring ToWideStr(const std::string& str) noexcept
	{
		int cch = ::MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), nullptr, 0);

		std::wstring wideStr;
		wideStr.resize(cch);
		::MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), wideStr.data(), cch);

		return wideStr;
	}

	std::string ToNarrowStr(const std::wstring& wstr) noexcept
	{
		int cch = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);

		std::string narrowStr;
		narrowStr.resize(cch);
		::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), &narrowStr[0], cch, nullptr, nullptr);

		return narrowStr;
	}

	std::string_view StringFromBytes(const byte* data, size_t size)
	{
		return std::string_view(reinterpret_cast<const char*>(data), size);
	}

	std::string HRToString(long hr)
	{
		HRESULT hres = static_cast<HRESULT>(hr);
		_com_error err(hres);
		return ToNarrowStr(err.ErrorMessage());
	}
}
