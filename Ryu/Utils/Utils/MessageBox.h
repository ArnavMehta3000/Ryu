#pragma once
#include "Common/StandardTypes.h"
#include <Windows.h>
#include <string>

namespace Ryu::Utils
{
	using namespace Common::StandardTypes;

	enum class MessagBoxButton : i32
	{
		AbortRetryIgnore       = MB_ABORTRETRYIGNORE,
		CancelTryAgainContinue = MB_CANCELTRYCONTINUE,
		Help                   = MB_HELP,
		Ok                     = MB_OK,
		OkCancel               = MB_OK,
		RetryCancel            = MB_RETRYCANCEL,
		YesNo                  = MB_YESNO,
		YesNoCancel            = MB_YESNOCANCEL
	};

	enum class MessageBoxIcon : i32
	{
		Error    = MB_ICONERROR,
		Question = MB_ICONQUESTION,
		Warning  = MB_ICONWARNING,
		Info     = MB_ICONINFORMATION
	};

	enum class MessageBoxReturnValue
	{
		Ok       = 1,
		Cancel   = 2,
		Abort    = 3,
		Retry    = 4,
		Ignore   = 5,
		Yes      = 6,
		No       = 7,
		TryAgain = 10,
		Continue = 11
	};

	struct MessageBoxFlags
	{
		MessagBoxButton Button{ MessagBoxButton::Ok };
		MessageBoxIcon Icon{ MessageBoxIcon::Info };

		constexpr i32 GetFlags() const
		{
			return static_cast<i32>(Button) | static_cast<i32>(Icon);
		}
	};

	struct MessageBoxDesc
	{
		HWND Window{ nullptr };
		std::string Title;
		std::string Text;
		MessageBoxFlags Flags{};
	};

	inline MessageBoxReturnValue ShowMessageBox(const MessageBoxDesc& desc)
	{
		return static_cast<MessageBoxReturnValue>(
			MessageBoxA(desc.Window, desc.Text.c_str(), desc.Title.c_str(), desc.Flags.GetFlags()));
	}
}
