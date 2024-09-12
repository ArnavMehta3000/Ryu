#pragma once
#include <Core/Includes.h>
#include <Engine/Core/Application.h>

namespace Ryu::Editor
{
	class RYU_API EditorApp : public Application
	{
	public:
		EditorApp(HINSTANCE hInst);
		~EditorApp();
	};
}