#pragma once
#include <CoreDLLDefines.h>
#include <Core/StandardTypes.h>
#include <Core/ObjectMacros.h>
#include <string>
#include <Windows.h>

namespace Ryu
{
	// Class inherited by every Win32 object (window/class)
	class RYU_API SubObject
	{
	public:
		RYU_DISABLE_COPY_AND_MOVE(SubObject);

		explicit SubObject(const std::wstring& className, const std::wstring& title, HINSTANCE hInst = ::GetModuleHandle(NULL), HICON icon = nullptr);
		virtual ~SubObject() = default;

		virtual bool Init() = 0;
		virtual bool RegisterNewClass();
		virtual LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		static LRESULT CALLBACK AssignMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		std::wstring m_className;
		std::wstring m_title;
		HINSTANCE    m_hInstance;
		HICON        m_icon;
	};
}