#pragma once
#include <Core/Plugin/PluginAPI.h>
#include <Plugins/Engine/RyuInput/InputEvents.h>
#include <Plugins/Engine/RyuInput/Devices/Keyboard.h>


namespace Ryu::Input
{
	class PLUGIN_API InputSystem : public IPlugin
	{
	public:
		InputSystem();
		virtual ~InputSystem();
		bool Initialize(const PluginAPI& api) override;
		void Shutdown() override;
		void SetInputCallbacks(const InputCallbacks& callbacks);

		const InputCallbacks& GetInputCallbacks() const;

		static LRESULT CALLBACK InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	private:
		InputCallbacks m_callbacks;
		Keyboard m_keyboard;
		HWND m_hWnd;
	};

	RYU_API_FUNC(void, SetInputCallbacks, const InputCallbacks& callbacks);
}