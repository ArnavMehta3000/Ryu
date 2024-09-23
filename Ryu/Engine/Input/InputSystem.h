#pragma once
#include <Core/Service/IService.h>
#include <Engine/Input/InputEvents.h>
#include <Engine/Input/Devices/Keyboard.h>
#include <Engine/Input/Devices/Mouse.h>

namespace Ryu
{
	class RYU_API IInputService : public IService
	{
	public:
		virtual ~IInputService() = default;
		virtual void AddInputCallbacks(const InputCallbacks& callbacks) = 0;
	};
	
	class RYU_API InputSystem : public ServiceBase<IInputService>
	{
	public:
		InputSystem(HWND window);
		virtual ~InputSystem();
		void AddInputCallbacks(const InputCallbacks& callbacks) override final;
		static LRESULT CALLBACK InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	private:
		WNDPROC                     m_originalWndProc;
		std::vector<InputCallbacks> m_callbacks;
		Keyboard                    m_keyboard;
		Mouse                       m_mouse;
		HWND                        m_hWnd;
	};
}
