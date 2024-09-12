#include "EditorApp.h"
#include <Editor/Internal/Log.h>

namespace Ryu::Editor
{
	EditorApp::EditorApp(HINSTANCE hInst) 
		: Application(Window::Config 
			{ 
				.Title = L"Ryu Editor",
				.Instance = hInst,
				.Type = Window::Type::Resizeable,
				.Width = 1280,
				.Height = 720,
			})
	{
		RYU_EDITOR_TRACE("EditorApp constructed");
	}

	EditorApp::~EditorApp()
	{
		RYU_EDITOR_TRACE("EditorApp destructed");
	}
}