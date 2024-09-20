#pragma once
#include <Core/Plugin/PluginAPI.h>
#include <Core/Application/IInputInterface.h>
#include <RyuInput/InputEvents.h>
#include <RyuInput/Devices/Keyboard.h>
#include <RyuInput/Devices/Mouse.h>

namespace Ryu::Input
{
	class PLUGIN_API InputSystem : public PluginBase<InputSystem>, public IInputInterface
	{
	public:
		InputSystem();
		virtual ~InputSystem();
		bool Initialize(const PluginAPI& api) override;
		void Shutdown() override;
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
