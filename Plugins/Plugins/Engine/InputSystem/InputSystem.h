#pragma once
#include <Core/Plugin/PluginAPI.h>

namespace Ryu
{
	class PLUGIN_API InputSystem : public IPlugin
	{
	public:
		InputSystem();
		virtual ~InputSystem();
		bool Initialize(const PluginAPI& api) override;
		void Shutdown() override;

		static LRESULT CALLBACK InputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_hWnd;
		static WNDPROC s_originalWndProc;
	};
}